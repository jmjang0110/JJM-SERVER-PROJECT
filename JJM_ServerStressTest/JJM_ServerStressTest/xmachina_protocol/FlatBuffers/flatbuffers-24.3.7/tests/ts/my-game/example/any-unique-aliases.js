// automatically generated by the FlatBuffers compiler, do not modify
/* eslint-disable @typescript-eslint/no-unused-vars, @typescript-eslint/no-explicit-any, @typescript-eslint/no-non-null-assertion */
import { Monster as MyGame_Example2_Monster } from '../../my-game/example2/monster.js';
import { Monster } from '../../my-game/example/monster.js';
import { TestSimpleTableWithEnum } from '../../my-game/example/test-simple-table-with-enum.js';
export var AnyUniqueAliases;
(function (AnyUniqueAliases) {
    AnyUniqueAliases[AnyUniqueAliases["NONE"] = 0] = "NONE";
    AnyUniqueAliases[AnyUniqueAliases["M"] = 1] = "M";
    AnyUniqueAliases[AnyUniqueAliases["TS"] = 2] = "TS";
    AnyUniqueAliases[AnyUniqueAliases["M2"] = 3] = "M2";
})(AnyUniqueAliases || (AnyUniqueAliases = {}));
export function unionToAnyUniqueAliases(type, accessor) {
    switch (AnyUniqueAliases[type]) {
        case 'NONE': return null;
        case 'M': return accessor(new Monster());
        case 'TS': return accessor(new TestSimpleTableWithEnum());
        case 'M2': return accessor(new MyGame_Example2_Monster());
        default: return null;
    }
}
export function unionListToAnyUniqueAliases(type, accessor, index) {
    switch (AnyUniqueAliases[type]) {
        case 'NONE': return null;
        case 'M': return accessor(index, new Monster());
        case 'TS': return accessor(index, new TestSimpleTableWithEnum());
        case 'M2': return accessor(index, new MyGame_Example2_Monster());
        default: return null;
    }
}
