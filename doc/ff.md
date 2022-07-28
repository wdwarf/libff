# Modules Note

## [ConfigService]

```cpp
class ConfigServer;
class ConfigClient;
```

### 数据定义

#### 参数值类型

| 参数值类型 | 长度  | 描述                      |
| ---------- | ----- | ------------------------- |
| bool       | 1byte | 布尔类型，只能取值 0 或 1 |
| int8       | 1byte |
| int16      | 2byte |
| int32      | 4byte |
| int64      | 8byte |
| uint8      | 1byte |
| uint16     | 2byte |
| uint32     | 4byte |
| uint64     | 8byte |
| string[N]  | N     | 字符串类型                |
| byte[N]    | N     | 字节数组                  |

#### 参数校验规则

##### 数值校验(N)

| 示例                          | 说明                                        |
| ----------------------------- | ------------------------------------------- |
| `"N:0..100"或"N:0..0x64"`     | 表示 0 到 100                               |
| `"N:0..100:2"或"N:0..0x64:2"` | 表示 0 到 100，步进为 2，即 0、2、4、...100 |
| `"N:0..100,200..300"`         | 表示 0 到 100 或 200 到 300                 |

##### 枚举类型(E)

| 示例                 | 说明                             |
| -------------------- | -------------------------------- |
| `"E:0,1"`            | 表示取值只能为 0 和 1            |
| `"E:enable,disable"` | 表示取值只能为 enable 和 disable |
| `"E:0x00,0x01"`      | 表示取值只能为 0 和 1            |

##### 正则表达式(R)

| 示例             | 说明                                |
| ---------------- | ----------------------------------- |
| `"R:^[0-9]{5}$"` | 表示一个由数字 0 到 9 组成了 5 位数 |

##### 函数校验(F)

| 示例                       | 说明                                             |
| -------------------------- | ------------------------------------------------ |
| `"F:SerialNumberValidate"` | 表示调用自定义函数 SerialNumberValidate 进行校验 |

#### 数据描述

```json
[
  // DeviceInfo
  {
    "name": "DeviceInfo", //参数名称
    "isSingle": true, //是否是单实例
    "isTemp": true, // 是否是临时参数
    "members": [
      //成员列表
      {
        "name": "serialNumber",
        "type": "string[32]",
        "isPK": false,
        "defaultValue": "",
        "validationRules": "R:^[0-9A-Z]{16}$"
      },
      {
        "name": "bootCount",
        "type": "uint32",
        "isPK": false,
        "defaultValue": "0",
        "validationRules": "N:0..0xffffff"
      },
      {
        "name": "available",
        "type": "bool",
        "isPK": false,
        "defaultValue": "false",
        "validationRules": ""
      }
    ]
  },
  // SysConfig
  {
    "name": "SysConfig", //参数名称
    "isSingle": false, //是否是单实例
    "isTemp": false, // 是否是临时参数
    "members": [
      //成员列表
      {
        "name": "variable",
        "type": "string[64]",
        "isPK": true,
        "defaultValue": "",
        "validationRules": ""
      },
      {
        "name": "value",
        "type": "string[128]",
        "isPK": false,
        "defaultValue": "",
        "validationRules": ""
      },
      {
        "name": "set_time",
        "type": "string[32]",
        "isPK": false,
        "defaultValue": "",
        "validationRules": ""
      }
    ],
    // 默认参数列表
    "defaultRows": [
      {
        "variable": "last_boot_time",
        "value": "2022-07-06 00:00:00"
      },
      {
        "variable": "log_level",
        "value": "info"
      }
    ]
  }
]
```

### 数据源

## 通讯协议

### 帧格式

| 帧头                    | 数据    |
| ----------------------- | ------- |
| sizeof(==DsPkgHeader==) | N bytes |

```cpp

#pragma pack(push,1)

struct DsPkgHeader{
  uint16_t magic = 0x6666;
  uint32_t length;
  uint32_t id;
  uint16_t frameType;
  uint16_t opCode;
  uint16_t option;
  uint16_t data_hash;
};

#pragma pack(pop)

```

### 帧类型

#### 帧类型码表

| 帧类型码 | 说明   |
| -------- | ------ |
| 0        | 请求帧 |
| 1        | 响应帧 |

### 操作码

#### 操作码表

| 操作码 | 说明             |
| ------ | ---------------- |
| 1      | 心跳             |
| 2      | 获取数据定义     |
| 3      | 获取数据名称列表 |
| 4      | 数据查询         |
| 5      | 数据添加         |
| 6      | 数据修改         |
| 7      | 数据删除         |

### 校验码

校验码使用==CRC-16/XMODEM==校验算法。
