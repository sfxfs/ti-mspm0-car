#ifndef JY901_H
#define JY901_H

#include "ti_msp_dl_config.h"

struct STime
{
    unsigned char ucYear;
    unsigned char ucMonth;
    unsigned char ucDay;
    unsigned char ucHour;
    unsigned char ucMinute;
    unsigned char ucSecond;
    unsigned short usMiliSecond;
};

/* 加速度 */
struct SAcc
{
    short a[3];
    short T;
};

/* 角速度 */
struct SGyro
{
    short w[3];
    short T;
};

/* 角度 */
struct SAngle
{
    short angle[6];
    short T;
};

/* 磁场 */
struct SMag
{
    short h[3];
    short T;
};

/* 连接状态 */
struct SDStatus
{
    short sDStatus[4];
};

/* 气压/海拔 */
struct SPress
{
    long lPressure;
    long lAltitude;
};

/* 经纬度 */
struct SLonLat
{
    long lLon;
    long lLat;
};

/* GPS */
struct SGPSV
{
    short sGPSHeight;
    short sGPSYaw;
    long lGPSVelocity;
};

/* 四元素 */
struct SQ
{
    short q[4];
};

/* JY901 原始数据 */
typedef struct
{
    struct STime stcTime;
    struct SAcc stcAcc;
    struct SGyro stcGyro;
    struct SAngle stcAngle;
    struct SMag stcMag;
    struct SDStatus stcDStatus;
    struct SPress stcPress;
    struct SLonLat stcLonLat;
    struct SGPSV stcGPSV;
    struct SQ stcQ;

} jy901_raw_t;

 //x、y、z 三维浮点
typedef struct
{
    float x;
    float y;
    float z;

} Vector3f;

//x、y、z 三维整形
typedef struct
{
    short x;
    short y;
    short z;

} Vector3s;

/* JY901 真实数据 */
typedef struct
{
    Vector3f acc;     //加速度
    Vector3f gyro;    //角速度
    Vector3f speed;   //地速
    Vector3s mag;     //磁场
    float pressure;   // 大气压
    float altitude;   // 海拔高度
    float roll;        //横滚角
    float pitch;       //俯仰角
    float yaw;         //航向角
    float temperature; //温度

} jy901_t;

void jy901_init(jy901_t *jy901);

#endif
