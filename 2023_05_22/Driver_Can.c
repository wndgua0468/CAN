/***********************************************************************/
/*Include*/ 
/***********************************************************************/
#include "Driver_Can.h"                    // Driver_Can.h 헤더 파일 포함


/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
// 전역 변수 선언
AppLedType        g_led;                                    // 전역 LED 구성 및 제어 구조체


/***********************************************************************/
/*Define*/ 
/***********************************************************************/


/***********************************************************************/
/*Typedef*/ 
/***********************************************************************/


/***********************************************************************/
/*Static Function Prototype*/ 
/***********************************************************************/




/* Macro to define Interrupt Service Routine.*/

IFX_INTERRUPT(canIsrTxHandler, 0, ISR_PRIORITY_CAN_TX);     // IFX_INTERRUPT 매크로를 사용하여 canIsrTxHandler 인터럽트 처리 함수 정의 및 우선순위 설정

/***********************************************************************/
/*Variable*/ 
/***********************************************************************/
// 변수 선언
App_MulticanBasic g_MulticanBasic;    // App_MulticanBasic 구조체의 전역 변수 선언/**< \brief Demo information */
volatile CanRxMsg rec;                // 수신된 CAN 메시지를 저장할 CanRxMsg 구조체 선언
int a[8]={0,0x1,0x2,0x3,0x4,0x5,0x6,0x7};   // 테스트 데이터 배열 선언
 
/***********************************************************************/
/*Function*/ 
/***********************************************************************/

void Driver_Can_Init(void)     // Driver_Can_Init 함수 정의
{
    // CAN 노드 구성 생성
    IfxMultican_Can_Config canConfig;
    IfxMultican_Can_initModuleConfig(&canConfig, &MODULE_CAN);

    /* 인터럽트 구성*/
    canConfig.nodePointer[TX_INTERRUPT_SRC_ID].priority = ISR_PRIORITY_CAN_TX;

    /*모듈 초기화*/
    IfxMultican_Can_initModule(&g_MulticanBasic.drivers.can, &canConfig);

    /*CAN 노드 구성 생성*/
    IfxMultican_Can_NodeConfig canNodeConfig;
    IfxMultican_Can_Node_initConfig(&canNodeConfig, &g_MulticanBasic.drivers.can);

    canNodeConfig.baudrate = 500000UL;                                                                       // CAN 속도를 500kbps로 설정
    {
        canNodeConfig.nodeId    = IfxMultican_NodeId_0;//(IfxMultican_NodeId)((int)IfxMultican_NodeId_0);    // CAN의 Node ID를 0으로 할당
        canNodeConfig.rxPin     = &IfxMultican_RXD0B_P20_7_IN;                                               // 입력 핀 설정
        canNodeConfig.rxPinMode = IfxPort_InputMode_pullUp;                                                  // 입력 핀 모드 설정
        canNodeConfig.txPin     = &IfxMultican_TXD0_P20_8_OUT;                                               // 출력 핀 설정
        canNodeConfig.txPinMode = IfxPort_OutputMode_pushPull;                                               // 출력 핀 모드 설정

        IfxMultican_Can_Node_init(&g_MulticanBasic.drivers.canSrcNode, &canNodeConfig);                     // CAN 노드 초기화
    }

    /* 메시지 객체 구성 생성*/
    IfxMultican_Can_MsgObjConfig canMsgObjConfig;                                                           // CAN 메시지 객체 구성 선언
    IfxMultican_Can_MsgObj_initConfig(&canMsgObjConfig, &g_MulticanBasic.drivers.canSrcNode);

    canMsgObjConfig.msgObjId              = 0;                                                               // 한 노드당 메시지 객체의 최대 개수는 256개
    canMsgObjConfig.messageId             = 0x100;                                                           // 메시지 ID 설정
    canMsgObjConfig.acceptanceMask        = 0x7FFFFFFFUL;                                                    // 수용 마스크 값 설정
    canMsgObjConfig.frame                 = IfxMultican_Frame_transmit;                                      // CAN TX 프레임 설정
    canMsgObjConfig.control.messageLen    = IfxMultican_DataLengthCode_8;                                    // 데이터 길이를 8로 설정
    canMsgObjConfig.control.extendedFrame = FALSE;                                                           // 확장 ID 사용 안 함
    canMsgObjConfig.control.matchingId    = TRUE;                                                            // 메시지 ID 일치를 활성화

    canMsgObjConfig.txInterrupt.enabled = TRUE;                                                               // TX 인터럽트를 활성화
    canMsgObjConfig.txInterrupt.srcId = TX_INTERRUPT_SRC_ID;                                                  // TX 인터럽트 소스 ID 설정

    /* initialize message object */
    IfxMultican_Can_MsgObj_init(&g_MulticanBasic.drivers.canSrcMsgObj, &canMsgObjConfig);        // CAN 메시지 객체 초기화
    // IO 포트 설정
    IfxPort_setPinModeOutput(STB, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);        // STB 핀을 출력 모드로 설정  // (push-pull 및 일반 출력 인덱스 사용)
    // CAN 칩의 STB 핀 설정 (저레벨 활성화)
    IfxPort_setPinLow(STB);

}


void Driver_Can_TxTest(void)             // Driver_Can_TxTest 함수 정의
{
    const uint32 dataLow  = 0x12340000;    // 하위 데이터 상수 설정
    const uint32 dataHigh = 0x9abc0000;    // 상위 데이터 상수 설정

    // 데이터 전송 시작
    {
        IfxMultican_Message msg;      // IfxMultican_Message 객체 msg 생성
        IfxMultican_Message_init(&msg, 0x100, dataLow, dataHigh, IfxMultican_DataLengthCode_8);    // 메시지 초기화 (ID: 0x100, 데이터 길이: 8)

        // 메시지 객체를 사용하여 메시지 전송 (전송 중일 때 계속 기다림)
        while (IfxMultican_Can_MsgObj_sendMessage(&g_MulticanBasic.drivers.canSrcMsgObj, &msg) == IfxMultican_Status_notSentBusy)
        {}
    }
}


void CAN_send(CanRxMsg *message)      // CAN_send 함수 정의, CanRxMsg 포인터를 인자로 받음
{


    IfxMultican_Message msg;           // IfxMultican_Message 객체 msg 생성

    // 데이터 하위 바이트 조합
    const unsigned dataLow = message->Data[0]|(message->Data[1]<<8)|(message->Data[2]<<16)|(message->Data[3]<<24);
    // 데이터 상위 바이트 조합
    const unsigned dataHigh = message->Data[4]|(message->Data[5]<<8)|(message->Data[6]<<16)|(message->Data[7]<<24);


    // 메시지 초기화 (ID, 데이터 길이 코드 설정)
    IfxMultican_Message_init(&msg,message->ID,dataLow,dataHigh,message->DLC);

    // 메시지 객체를 사용하여 메시지 전송 (전송 중일 때 계속 기다림)
    while (IfxMultican_Can_MsgObj_sendMessage(&g_MulticanBasic.drivers.canSrcMsgObj, &msg) == IfxMultican_Status_notSentBusy)
    {

    }
}


void CAN_TEST(void)          // CAN_TEST 함수 정의
{
    CanRxMsg MES;            // CanRxMsg 객체 MES 생성
    int i=0;                 // 정수 i 초기화
    MES.ID=0x890;            // 메시지 ID 설정
    MES.IDE=0;               // 메시지 확장 ID 비활성화
    MES.DLC=8;               // 데이터 길이 설정

    for(i=0; i<8; i++)       // i가 0부터 7까지 반복
    {
        MES.Data[i]=a[i];    // a 배열의 데이터 복사
    }
    CAN_send(&MES);          // CAN_send 함수를 사용하여 메시지 전송
}


/* Interrupt Service Routine (ISR) called once the TX interrupt has been generated.
 * Turns on the LED1 to indicate successful CAN message transmission.
 */
// TX 인터럽트가 생성되면 호출되는 인터럽트 서비스 루틴 (ISR)
void canIsrTxHandler(void)
{
    // CAN 메시지가 전송되었음을 나타내기 위해 LED1 켜기
    blinkLED1();

    //IfxPort_setPinLow(g_led.led1.port, g_led.led1.pinIndex);
}


void initLed(void)   // LED 초기화 함수 정의
{
    /* ======================================================================
     * Configuration of the pins connected to the LEDs:
     * ======================================================================
     *  - define the GPIO port
     *  - define the GPIO pin that is the connected to the LED
     *  - define the general GPIO pin usage (no alternate function used)
     *  - define the pad driver strength
     * ======================================================================
     */
    // LED에 연결된 핀 설정
    g_led.led1.port      = &MODULE_P00;                                          // GPIO 포트 설정
    g_led.led1.pinIndex  = PIN5;                                                 // LED에 연결된 GPIO 핀 번호 설정
    g_led.led1.mode      = IfxPort_OutputIdx_general;                            // 일반 GPIO 핀 사용 설정 (대체 기능 사용 안 함)
    g_led.led1.padDriver = IfxPort_PadDriver_cmosAutomotiveSpeed1;               // 패드 드라이버 강도 설정


    // 같은 과정을 LED2에 대해서도 수행합니다.
    g_led.led2.port      = &MODULE_P00;
    g_led.led2.pinIndex  = PIN6;
    g_led.led2.mode      = IfxPort_OutputIdx_general;
    g_led.led2.padDriver = IfxPort_PadDriver_cmosAutomotiveSpeed1;

    // LED에 연결된 핀들을 기본 상태로 "높게" 설정합니다.
    IfxPort_setPinHigh(g_led.led1.port, g_led.led1.pinIndex);
    IfxPort_setPinHigh(g_led.led2.port, g_led.led2.pinIndex);
    // 두 LED에 연결된 핀들의 입력/출력 모드를 설정합니다.
    IfxPort_setPinModeOutput(g_led.led1.port, g_led.led1.pinIndex, IfxPort_OutputMode_pushPull, g_led.led1.mode);
    IfxPort_setPinModeOutput(g_led.led2.port, g_led.led2.pinIndex, IfxPort_OutputMode_pushPull, g_led.led2.mode);

    // 두 LED에 연결된 핀들의 패드 드라이버 모드를 설정합니다.
    IfxPort_setPinPadDriver(g_led.led1.port, g_led.led1.pinIndex, g_led.led1.padDriver);
    IfxPort_setPinPadDriver(g_led.led2.port, g_led.led2.pinIndex, g_led.led2.padDriver);
}


void blinkLED1(void)          // LED1 깜빡이기 함수 정의
{

    IfxPort_setPinHigh(LED1);                                                         // LED1 켜기
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME_1ms));      // 1 밀리초 대기
    IfxPort_setPinLow(LED1);                                                          // LED1 끄기
}
void blinkLED2(void)          // LED2 깜빡이기 함수 정의
{
    IfxPort_togglePin(LED2);                                                            // LED2 상태 전환
    waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, WAIT_TIME_10ms));       // 500 밀리초 대기
}
