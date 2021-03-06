# Open-source Smart House System Protocol CAN Frame Format

## Extended Identifier Format
| Start bit | End bit | Name              | Description |
| --------- | ------- | ----------------- | ----------- |
| 0x00      | 0x00    | NOT_ERROR_FLAG    | If this bit is low, the frame is considered to be an error frame. |
| 0x01      | 0x01    | START_FRAME_FLAG* | If this bit is high, the frame is considered to be the first frame of a packet. |
| 0x02      | 0x02    | MULTI_FRAME_FLAG  | If this bit is high, the frame is considered to be only a part of a packet. |
| 0x03      | 0x08    | RESERVED          | Reserved |
| 0x09      | 0x0C    | LAST_FRAME_ID**   | Most significant nibble (0xf00) of the last frame id inside current packet. |
|           |         | FRAME_ID          | Most significant nibble (0xf00) of the current frame id. |
| 0x0D      | 0x1C    | TRANSMITTER_MAC   | Transmitter device MAC address. |

> \* If the MULTI_FRAME_FLAG is not set, START_FRAME_FLAG must be set.

> \*\* If the START_FRAME_FLAG is set, FRAME_COUNT should be provided, otherwise FRAME_ID should be provided. 

## Frame Format

### Error Packet
* NOT_ERROR_FLAG = 0
* START_FRAME_FLAG = 1
* MULTI_FRAME_FLAG = 0
* FRAME_COUNT = 0x01

| Start byte | End byte | Name | Description |
| ---------  | -------  | ---- | ----------- |
| 0x00       | 0x07     | DATA | Serialized packet. |

### Single Frame Packet
* NOT_ERROR_FLAG = 1
* START_FRAME_FLAG = 1
* MULTI_FRAME_FLAG = 0
* FRAME_COUNT = 0x01

| Start byte | End byte | Name | Description |
| ---------  | -------  | ---- | ----------- |
| 0x00       | 0x07     | DATA | Serialized packet. |

### Multi Frame Packet

#### First Frame
* NOT_ERROR_FLAG = 1
* START_FRAME_FLAG = 1
* MULTI_FRAME_FLAG = 1

| Start byte | End byte | Name          | Description |
| ---------  | -------  | ------------- | ----------- |
| 0x00       | 0x00     | LAST_FRAME_ID | Least significant byte (0x0ff) of the last frame id inside current packet. |
| 0x01       | 0x07     | DATA          | Serialized packet. |

#### Successive Frames
* NOT_ERROR_FLAG = 1
* START_FRAME_FLAG = 0
* MULTI_FRAME_FLAG = 1

| Start byte | End byte | Name        | Description |
| ---------  | -------  | ----------- | ----------- |
| 0x00       | 0x00     | FRAME_ID    | Least significant byte (0x0ff) of the current frame id. |
| 0x01       | 0x07     | DATA        | Serialized packet. |

## Navigation
* [README](../README.md)
* CAN frame format
* [USART frame format](USART.md)
* [Event packet format](PACKET.md)
