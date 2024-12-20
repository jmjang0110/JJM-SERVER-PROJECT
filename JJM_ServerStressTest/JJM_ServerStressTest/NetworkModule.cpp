#include "pch.h"
#include "NetworkModule.h"
#include "Session.h"
#include "ExOverlapped.h"
#include "Win32RenderMgr.h"


std::array<Session, MAX_CLIENT> NetworkModule::m_Sessions;
std::array<LONG64, MAX_CLIENT>		NetworkModule::m_Client_ID_map;

NetworkModule::~NetworkModule() 
{
	if (m_NetworkStart) {
		for (auto& t : m_tWorkers)
			t.join();
		m_tTest.join();
	}

	::FreeConsole();
	::fclose(mConsole);
}

void NetworkModule::Init()
{
	std::wcout.imbue(std::locale("korean"));
	WSADATA wsaData;
	int ret = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != S_OK)
		assert(0);

	// IOCP ��������.
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0); 
	if (m_hIOCP == NULL)
		assert(0);

	::AllocConsole();
	freopen_s(&mConsole, "CONOUT$", "w", stdout); // ǥ�� ����� �ַܼ� ���𷺼�
	freopen_s(&mConsole, "CONIN$", "r", stdin);	// ǥ�� �Է��� �ַܼ� ���𷺼�
	m_Last_connected_time = Clock::now();

}

void NetworkModule::WorkerThread()
{

	CompletionTask iocp_task;
	while (m_NetworkStart) {
		BOOL ret = GQCS(iocp_task);
		if (ret == FALSE) {
			int err = ::WSAGetLastError();
			if (err != ERROR_IO_PENDING) {
				PrintErrorDescription(err);
				Disconnect_Session_FromServer(iocp_task.key_ID);
				continue;
			}
		}
		Process_CompletionTask(iocp_task.key_ID, static_cast<int>(iocp_task.bytes), iocp_task.exOver);
	}
}

void NetworkModule::TestThread()
{
	constexpr float SendInterval_CPkt_Transform = 1.f; // 0.1�� ���� (10Hz)
	const auto sendIntervalDuration = std::chrono::duration<float>(SendInterval_CPkt_Transform);

	auto lastSendTime_CPkt_Transform = std::chrono::steady_clock::now();

	while (m_NetworkStart) {
		Try_Connect_Session_ToServer();

		auto currTime = std::chrono::steady_clock::now();

		// ��Ŷ ���� ���� Ȯ�� �� ����
		if (currTime - lastSendTime_CPkt_Transform >= sendIntervalDuration) {
			for (int i = 0; i < m_Connected_clients_num; ++i) {
				if (!m_Sessions[i].m_IsConnected) continue; // ���� ����� ������ �н�

				m_Sessions[i].Send_CPkt_Transform();
			}
			// ������ ���� �ð� ���� (���� �ð��� ���)
			lastSendTime_CPkt_Transform = currTime;
		}
	}
}


void NetworkModule::Execute(int workerThread_num)
{
	m_NetworkStart = true;
	for (int i = 0; i < workerThread_num; ++i) {
		m_tWorkers.emplace_back(&NetworkModule::WorkerThread, this);
	}
	m_tTest = std::thread{ &NetworkModule::TestThread, this };
}

void NetworkModule::Connect_Session_ToServer(LONG64 ID)
{
	// Connect ! 
	m_Sessions[ID].Init(ID);
	bool ret = m_Sessions[ID].Connect(SERVER_IP, SERVER_PORT);
	if (ret == false) {
		return;
	}
	m_Connected_clients_num++;
	m_Active_clients_num++;
	m_Sessions[ID].CreateIOCP(m_hIOCP, static_cast<ULONG_PTR>(ID), 0);
	m_Last_connected_time = Clock::now();

	if (!m_Sessions[ID].Send_CPkt_LogIn(std::to_string(ID), "1234")) {
		std::cout << ID << " Send Failed\n";
		return;
	}
	m_Sessions[ID].DoRecv();

	// 0 Room : 0 ~ 499
	// 1 Room : 500 ~ 599 
	// ...
	if (ID > 0 && (ID + 1) % MAX_CLIENT_PER_ROOM == 0)
		m_Sessions[ID].Send_CPkt_PlayGame();

}

void NetworkModule::Exit()
{
	m_NetworkStart = false;
}

void NetworkModule::Process_CompletionTask(LONG64 id, int bytes, ExOverlapped* over)
{
	switch (over->m_IOtype)
	{
	case IO_TYPE::READ:
	{
		if (m_Sessions[id].OnRecv(bytes, over) == false)
			Disconnect_Session_FromServer(id);
	}
	break;
	case IO_TYPE::WRITE:
	{
		if (m_Sessions[id].OnSend(bytes, over) == false)
			Disconnect_Session_FromServer(id);
	}
	break;
	default:
		break;
	}
}


bool NetworkModule::GQCS(CompletionTask& ct)
{
	DWORD waitTime = INFINITE;
	WSAOVERLAPPED* over{};
	ct.success = ::GetQueuedCompletionStatus(m_hIOCP
										  , &ct.bytes
										  , (PULONG_PTR)&ct.key_ID
										  , &over
										  , waitTime);

	ct.exOver = reinterpret_cast<ExOverlapped*>(over);
	return ct.success;
}

void NetworkModule::Try_Connect_Session_ToServer()
{
	static int connect_time_val = 1;

	// m_Active_clients_num �� UINT64 ���� -1  �϶� MAX_USER ���� ũ�ٰ� ó���ƾ�����..

	// ���� ������ ���� ���� ���� 
	LONG64 activeclientsNum = m_Active_clients_num.load();
	if (activeclientsNum >= MAX_USER)
		return;

	// connected_client_num = session ID = session Index �̹Ƿ� ���� ����� �D�����.
	if (m_Connected_clients_num.load() >= MAX_CLIENT)
		return;

	if (m_Active_clients_num == 0)
		m_delay = 0;

	/* Session�� Server�� Connect �õ����� (�����̿� ���� ����) */
	int deltaT = std::chrono::duration_cast<ms>(Clock::now() - m_Last_connected_time).count();
	if (CONNECT_DELAY > deltaT) {
		return;
	}

	// �����̰� LIMIT ���� Ŀ���� ���� ���� 
	int delay = m_delay;
	if (delay >= LIMIT_DELAY) {
		m_Last_connected_time = Clock::now();
		Disconnect_Session_FromServer(m_close_ID.load());
		return;
	}
	else if (delay >= LIMIT_DELAY / 2) {
		connect_time_val = 20;
	}
	else {
		connect_time_val = connect_time_val < 2 ? 1 : connect_time_val - 1;
	}

	if (connect_time_val * CONNECT_DELAY > deltaT)
		return;


	LONG64 ID = m_Connected_clients_num;
	Connect_Session_ToServer(ID);
}

void NetworkModule::Disconnect_Session_FromServer(LONG64 ID)
{

	// Session�� Connect�Ǿ��ִ� ���¸� false�� atomic �ϰ� ���� 
	bool status = true;
	if (std::atomic_compare_exchange_strong(&m_Sessions[ID].m_IsConnected, &status, false))
	{
		// Disconnecct ���� ��Ŷ ������ 

		m_Sessions[ID].Disconnect();
		m_Active_clients_num--;
		m_close_ID++;
	}
}


void NetworkModule::PrintErrorDescription(int errorCode) {
	static std::unordered_map<int, std::string> errorDescriptions = {
		{258, "WAIT_TIMEOUT: �۾��� ������ �ð� ���� �Ϸ���� �ʾҽ��ϴ�."},
		{10004, "WSAEINTR: ���ܵ� �Լ� ȣ���� ��ҵǾ����ϴ�."},
		{10009, "WSAEBADF: �߸��� ���� �ڵ��Դϴ�."},
		{10013, "WSAEACCES: ��û�� �۾��� ������ �ʽ��ϴ�."},
		{10014, "WSAEFAULT: �߸��� �ּҰ� ���Ǿ����ϴ�."},
		{10022, "WSAEINVAL: �߸��� �Ű������� ���޵Ǿ����ϴ�."},
		{10024, "WSAEMFILE: �� �� �ִ� ������ �ִ� ������ �ʰ��߽��ϴ�."},
		{10035, "WSAEWOULDBLOCK: �۾��� �񵿱�� ���� ���̸� �Ϸ���� �ʾҽ��ϴ�."},
		{10036, "WSAEINPROGRESS: ���� ���ܵ� �۾��� ���� ���Դϴ�."},
		{10037, "WSAEALREADY: �̹� ��û�� ���� ���Դϴ�."},
		{10038, "WSAENOTSOCK: ������ �ƴ� �ڵ��� ���޵Ǿ����ϴ�."},
		{10048, "WSAEADDRINUSE: �ּҰ� �̹� ��� ���Դϴ�."},
		{10049, "WSAEADDRNOTAVAIL: ��û�� �ּҸ� ����� �� �����ϴ�."},
		{10050, "WSAENETDOWN: ��Ʈ��ũ�� �ٿ�Ǿ����ϴ�."},
		{10051, "WSAENETUNREACH: ��Ʈ��ũ�� ������ �� �����ϴ�."},
		{10052, "WSAENETRESET: ������ ��Ʈ��ũ���� ���µǾ����ϴ�."},
		{10053, "WSAECONNABORTED: ����Ʈ����� ���� ������ �ߴܵǾ����ϴ�."},
		{10054, "WSAECONNRESET: ���� ȣ��Ʈ�� ���� ������ ������ ���µǾ����ϴ�."},
		{10055, "WSAENOBUFS: ���� ������ �����մϴ�."},
		{10056, "WSAEISCONN: ������ �̹� ����� �����Դϴ�."},
		{10057, "WSAENOTCONN: ������ ������� �ʾҽ��ϴ�."},
		{10058, "WSAESHUTDOWN: ������ ����Ǿ� �����͸� ���� �� �����ϴ�."},
		{10060, "WSAETIMEDOUT: ���� �õ��� �ð� �ʰ��Ǿ����ϴ�."},
		{10061, "WSAECONNREFUSED: ���� �õ��� �źεǾ����ϴ�."},
		{10064, "WSAEHOSTDOWN: ȣ��Ʈ�� �ٿ�Ǿ����ϴ�."},
		{10065, "WSAEHOSTUNREACH: ȣ��Ʈ�� ������ �� �����ϴ�."},
		{10067, "WSAEPROCLIM: ���μ����� �ʹ� �����ϴ�."}
	};

	auto it = errorDescriptions.find(errorCode);
	if (it != errorDescriptions.end()) {
		std::cout << "���� �ڵ�: " << errorCode << ", ����: " << it->second << std::endl;
	}
	else {
		std::cout << "���� �ڵ�: " << errorCode << ", ����: �� �� ���� �����Դϴ�." << std::endl;
	}
}

void NetworkModule::Draw_Sessions()
{
	const int cols = 5;                      // ���� �� ����
	const int rows = 2;                      // ���� �� ����
	const int roomWidth = ROOM_WIDTH;               // �� ���� �ʺ�
	const int roomHeight = ROOM_HEIGHT;              // �� ���� ����
	const int margin = 20;                   // �� �� ����
	const int startX = 50;                   // �� ���� X ��ǥ
	const int startY = 100;                  // �� ���� Y ��ǥ
	const int maxClientsPerRoom = MAX_CLIENT_PER_ROOM; // ��� �ִ� Ŭ���̾�Ʈ ��

	for (LONG64 clientID = 0; clientID < m_Connected_clients_num; ++clientID)
	{
		// �� ��ȣ ��� (0���� ����)
		int roomIndex = clientID / maxClientsPerRoom;

		// ���� �������� �ʴ� ��� ����
		if (roomIndex >= (cols * rows)) {
			continue;
		}

		// ���� ��� �� ���
		int roomRow = roomIndex / cols;
		int roomCol = roomIndex % cols;

		// ���� ���� ��� ��ǥ
		int roomX = startX + roomCol * (roomWidth + margin);
		int roomY = startY + roomRow * (roomHeight + margin);

		// �� �ȿ����� ���� ��ġ ���� (������ ��ġ�� �ʵ���)
		int dotX = roomX + (rand() % (roomWidth - 10)) + 5; // 5~(roomWidth-5)
		int dotY = roomY + (rand() % (roomHeight - 10)) + 5; // 5~(roomHeight-5)

		// �� �׸���
		Win32RenderMgr::GetInstance()->DrawFilledRectangle(
			POINT{ dotX, dotY },
			3, 3, // ���� ũ��
			RGB(200, 100, 0) 
		);
	}
}

int NetworkModule::GetClientsInRoom(int roomid, int maxclientsNum)
{
	int result = 0;

	// �濡 �ش��ϴ� ���� ���� ���
	int startIdx = roomid * maxclientsNum;
	int endIdx = startIdx + maxclientsNum;

	// ������ ����� �ʵ��� MAX_CLIENT Ȯ��
	if (startIdx >= MAX_CLIENT)
		return 0;

	if (endIdx > MAX_CLIENT)
		endIdx = MAX_CLIENT;

	// �濡 ���� ������ ���� ���¸� Ȯ��
	for (int i = startIdx; i < endIdx; ++i)
	{
		if (m_Sessions[i].m_IsConnected)
		{
			++result;
		}

		// ����ȭ: �̹� �ִ� �ο��� �ʰ��ϸ� �� �̻� �˻��� �ʿ䰡 ����
		if (result >= maxclientsNum)
			break;
	}

	return result;
}

