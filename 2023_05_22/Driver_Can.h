#ifndef DRIVER_CAN
#define DRIVER_CAN

/***********************************************************************/
/*Include*/ 
/***********************************************************************/
// 필요한 헤더 파일 포함
#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxMultican_Can.h"
#include "IfxPort.h"
#include "Bsp.h"


/***********************************************************************/
/*Define*/ 
/***********************************************************************/
// 상수 정의
#define STB         &MODULE_P20,6

// TX 인터럽트 구성
#define TX_INTERRUPT_SRC_ID         IfxMultican_SrcId_0          // 전송 인터럽트 서비스 요청 ID
#define ISR_PRIORITY_CAN_TX         2                           // CAN TX 인터럽트 우선순위 정의
// PIN 정의
#define PIN5                        5                            // TX ISR에서 사용되는 LED1은 이 핀에 연결됩니다.
#define PIN6                        6                            // RX ISR에서 사용되는 LED2은 이 핀에 연결됩니다.


// 대기 시간 상수 (밀리초 단위)
#define WAIT_TIME_1ms               1
#define WAIT_TIME_5ms               5
#define WAIT_TIME_10ms              10
#define WAIT_TIME_20ms              20
#define WAIT_TIME_50ms              50
#define WAIT_TIME_100ms             100

// LED 핀 정의
#define LED1         &MODULE_P00,5
#define LED2         &MODULE_P00,6
/***********************************************************************/
/*Typedef*/ 
/***********************************************************************/
// App_MulticanBasic 구조체 정의
typedef struct
{
    struct
    {
        IfxMultican_Can        can;           // CAN 드라이버 핸들
        IfxMultican_Can_Node   canSrcNode;    // CAN 소스 노드
        IfxMultican_Can_MsgObj canSrcMsgObj;  // CAN 소스 메시지 객체
    }drivers;
} App_MulticanBasic;

// CanRxMsg 구조체 정의
typedef struct
{
    unsigned long ID;                           // CAN 메시지의 식별자
    unsigned char IDE;                          // CAN 메시지의 확장 식별자
    unsigned char DLC;                          // CAN 메시지의 데이터 길이 코드
    unsigned char Data[8];                      // CAN 메시지의 데이터 배열 (최대 8바이트)
} CanRxMsg;

// AppLedType 구조체 정의
typedef struct
{
    IfxPort_Pin_Config              led1;                   // LED1 구성 구조체
    IfxPort_Pin_Config              led2;                  // LED2 구성 구조체
} AppLedType;


/***********************************************************************/
/*External Variable*/ 
/***********************************************************************/
IFX_EXTERN App_MulticanBasic g_MulticanBasic;    // 전역 변수로 App_MulticanBasic 구조체 선언


/***********************************************************************/
/*Global Function Prototype*/ 
/***********************************************************************/
// 전역 함수 원형 선언
extern void Driver_Can_Init(void);                  // Driver_Can_Init 함수: CAN 드라이버 초기화
extern void Driver_Can_TxTest(void);                // Driver_Can_TxTest 함수: CAN 테스트 전송
extern void CAN_send(CanRxMsg *message);            // CAN_send 함수: CAN 메시지 전송
extern void CAN_TEST(void);                         // CAN_TEST 함수: CAN 테스트 실행
void initLed(void);                                 // initLed 함수: LED 초기화
void blinkLED1(void);                               // blinkLED1 함수: LED1 불빛
void blinkLED2(void);                             // blinkLED2 함수: LED2 불빛
#endif /* DRIVER_STM */
