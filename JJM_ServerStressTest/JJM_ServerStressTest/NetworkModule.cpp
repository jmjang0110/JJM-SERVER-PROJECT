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

	// IOCP 생성하자.
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0); 
	if (m_hIOCP == NULL)
		assert(0);

	::AllocConsole();
	freopen_s(&mConsole, "CONOUT$", "w", stdout); // 표준 출력을 콘솔로 리디렉션
	freopen_s(&mConsole, "CONIN$", "r", stdin);	// 표준 입력을 콘솔로 리디렉션
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
	constexpr float SendInterval_CPkt_Transform = 1.f; // 0.1초 간격 (10Hz)
	const auto sendIntervalDuration = std::chrono::duration<float>(SendInterval_CPkt_Transform);

	auto lastSendTime_CPkt_Transform = std::chrono::steady_clock::now();

	while (m_NetworkStart) {
		Try_Connect_Session_ToServer();

		auto currTime = std::chrono::steady_clock::now();

		// 패킷 전송 간격 확인 및 전송
		if (currTime - lastSendTime_CPkt_Transform >= sendIntervalDuration) {
			for (int i = 0; i < m_Connected_clients_num; ++i) {
				if (!m_Sessions[i].m_IsConnected) continue; // 접속 종료된 세션은 패스

				m_Sessions[i].Send_CPkt_Transform();
			}
			// 마지막 전송 시간 갱신 (현재 시간을 사용)
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

	// m_Active_clients_num 이 UINT64 여서 -1  일때 MAX_USER 보다 크다고 처리됐었ㅇ므..

	// 동접 끝까지 차면 접속 끊기 
	LONG64 activeclientsNum = m_Active_clients_num.load();
	if (activeclientsNum >= MAX_USER)
		return;

	// connected_client_num = session ID = session Index 이므로 범위 벗어나면 귾어야함.
	if (m_Connected_clients_num.load() >= MAX_CLIENT)
		return;

	if (m_Active_clients_num == 0)
		m_delay = 0;

	/* Session을 Server에 Connect 시도하자 (딜레이에 따라 조정) */
	int deltaT = std::chrono::duration_cast<ms>(Clock::now() - m_Last_connected_time).count();
	if (CONNECT_DELAY > deltaT) {
		return;
	}

	// 딜레이가 LIMIT 보다 커지면 접속 끊기 
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

	// Session이 Connect되어있는 상태를 false로 atomic 하게 변경 
	bool status = true;
	if (std::atomic_compare_exchange_strong(&m_Sessions[ID].m_IsConnected, &status, false))
	{
		// Disconnecct 관련 패킷 보내기 

		m_Sessions[ID].Disconnect();
		m_Active_clients_num--;
		m_close_ID++;
	}
}


void NetworkModule::PrintErrorDescription(int errorCode) {
	static std::unordered_map<int, std::string> errorDescriptions = {
		{258, "WAIT_TIMEOUT: 작업이 지정된 시간 내에 완료되지 않았습니다."},
		{10004, "WSAEINTR: 차단된 함수 호출이 취소되었습니다."},
		{10009, "WSAEBADF: 잘못된 파일 핸들입니다."},
		{10013, "WSAEACCES: 요청한 작업이 허용되지 않습니다."},
		{10014, "WSAEFAULT: 잘못된 주소가 사용되었습니다."},
		{10022, "WSAEINVAL: 잘못된 매개변수가 전달되었습니다."},
		{10024, "WSAEMFILE: 열 수 있는 소켓의 최대 개수를 초과했습니다."},
		{10035, "WSAEWOULDBLOCK: 작업이 비동기로 실행 중이며 완료되지 않았습니다."},
		{10036, "WSAEINPROGRESS: 현재 차단된 작업이 진행 중입니다."},
		{10037, "WSAEALREADY: 이미 요청이 진행 중입니다."},
		{10038, "WSAENOTSOCK: 소켓이 아닌 핸들이 전달되었습니다."},
		{10048, "WSAEADDRINUSE: 주소가 이미 사용 중입니다."},
		{10049, "WSAEADDRNOTAVAIL: 요청한 주소를 사용할 수 없습니다."},
		{10050, "WSAENETDOWN: 네트워크가 다운되었습니다."},
		{10051, "WSAENETUNREACH: 네트워크에 접근할 수 없습니다."},
		{10052, "WSAENETRESET: 연결이 네트워크에서 리셋되었습니다."},
		{10053, "WSAECONNABORTED: 소프트웨어로 인해 연결이 중단되었습니다."},
		{10054, "WSAECONNRESET: 원격 호스트에 의해 연결이 강제로 리셋되었습니다."},
		{10055, "WSAENOBUFS: 버퍼 공간이 부족합니다."},
		{10056, "WSAEISCONN: 소켓이 이미 연결된 상태입니다."},
		{10057, "WSAENOTCONN: 소켓이 연결되지 않았습니다."},
		{10058, "WSAESHUTDOWN: 소켓이 종료되어 데이터를 보낼 수 없습니다."},
		{10060, "WSAETIMEDOUT: 연결 시도가 시간 초과되었습니다."},
		{10061, "WSAECONNREFUSED: 연결 시도가 거부되었습니다."},
		{10064, "WSAEHOSTDOWN: 호스트가 다운되었습니다."},
		{10065, "WSAEHOSTUNREACH: 호스트에 접근할 수 없습니다."},
		{10067, "WSAEPROCLIM: 프로세스가 너무 많습니다."}
	};

	auto it = errorDescriptions.find(errorCode);
	if (it != errorDescriptions.end()) {
		std::cout << "에러 코드: " << errorCode << ", 설명: " << it->second << std::endl;
	}
	else {
		std::cout << "에러 코드: " << errorCode << ", 설명: 알 수 없는 에러입니다." << std::endl;
	}
}

void NetworkModule::Draw_Sessions()
{
	const int cols = 5;                      // 방의 열 개수
	const int rows = 2;                      // 방의 행 개수
	const int roomWidth = ROOM_WIDTH;               // 각 방의 너비
	const int roomHeight = ROOM_HEIGHT;              // 각 방의 높이
	const int margin = 20;                   // 방 간 간격
	const int startX = 50;                   // 방 시작 X 좌표
	const int startY = 100;                  // 방 시작 Y 좌표
	const int maxClientsPerRoom = MAX_CLIENT_PER_ROOM; // 방당 최대 클라이언트 수

	for (LONG64 clientID = 0; clientID < m_Connected_clients_num; ++clientID)
	{
		// 방 번호 계산 (0부터 시작)
		int roomIndex = clientID / maxClientsPerRoom;

		// 방이 존재하지 않는 경우 무시
		if (roomIndex >= (cols * rows)) {
			continue;
		}

		// 방의 행과 열 계산
		int roomRow = roomIndex / cols;
		int roomCol = roomIndex % cols;

		// 방의 좌측 상단 좌표
		int roomX = startX + roomCol * (roomWidth + margin);
		int roomY = startY + roomRow * (roomHeight + margin);

		// 방 안에서의 랜덤 위치 지정 (점들이 겹치지 않도록)
		int dotX = roomX + (rand() % (roomWidth - 10)) + 5; // 5~(roomWidth-5)
		int dotY = roomY + (rand() % (roomHeight - 10)) + 5; // 5~(roomHeight-5)

		// 점 그리기
		Win32RenderMgr::GetInstance()->DrawFilledRectangle(
			POINT{ dotX, dotY },
			3, 3, // 점의 크기
			RGB(200, 100, 0) 
		);
	}
}

int NetworkModule::GetClientsInRoom(int roomid, int maxclientsNum)
{
	int result = 0;

	// 방에 해당하는 세션 범위 계산
	int startIdx = roomid * maxclientsNum;
	int endIdx = startIdx + maxclientsNum;

	// 범위를 벗어나지 않도록 MAX_CLIENT 확인
	if (startIdx >= MAX_CLIENT)
		return 0;

	if (endIdx > MAX_CLIENT)
		endIdx = MAX_CLIENT;

	// 방에 속한 세션의 연결 상태를 확인
	for (int i = startIdx; i < endIdx; ++i)
	{
		if (m_Sessions[i].m_IsConnected)
		{
			++result;
		}

		// 최적화: 이미 최대 인원을 초과하면 더 이상 검사할 필요가 없음
		if (result >= maxclientsNum)
			break;
	}

	return result;
}

