/**************************************************************
* Description  : Q560 4G����ģ������
* Author       : 2018/6/4 ����һ, by redmorningcn
*/

/*******************************************************************************
* INCLUDES
*/
#include <includes.h>
#include <global.h>
#include <string.h>
#include <stdlib.h> 
#include <bsp_q560.h>

#define BSP_Q560_4G_EN 1
#if     BSP_Q560_4G_EN > 0
/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */

/*******************************************************************************
 * GLOBAL FUNCTIONS
 */

/*******************************************************************************
 * EXTERN VARIABLES
 */

/*******************************************************************************
 * EXTERN FUNCTIONS
 */

//������Ҫ��ʶһ��GPS������ϵ��GPS����ϵ��ѭWGS-84��׼���������׼�£�GPSоƬ����
//������ͬ�����ݰ���ʽ������������֡֡ͷ�Ĳ�ͬ��GPS���ݿ��Է���ΪGPGGA��GPGSA��GPGSV��GPRMC�ȡ�
//��Щ֡ͷ��ʶ�˺���֡�����ݵ���ɽṹ��ͨ������£����������ĵĶ�λ�����羭γ�ȡ�
//�ٶȡ�ʱ��Ⱦ����Դ�GPRMC֡�л�ȡ�õ���
//�ڴ��Ҳ���������֡�ĸ�ʽ�������Ͽ��Ժ������ѵ���Ҳ������оƬ���׵Ľӿ��ĵ��ҵ�����֡�ĸ�ʽ��
//ͨ�����GPRMC�������ǿ��Եõ������ڡ�3040.8639,N,10405.7573,E����λ�����ݣ�
//���ֶκ���������ĵ����ҵ��������ҽ����Ľ���ǣ���γ30��40.8639'������104��5.7573'��
//�ڵõ����������ݺ����ǿ�������ȥ�ڵ�ͼ�϶�λһ�£����Ƿ��ֶ�����ͼ��λ�õ���С����ʽ�����ꡣ
//�������Ǿ���Ҫȥ����ΰ�һ��GPS��ԭʼ���ݣ�ת��ΪС����ʽ�����ݡ�
//������106��14'15"��ת��
//��Ϊ�ȷ��붼����ʮ���Ƶ�
//���Կ�������ת����
//15/60=0.25��
//(14+0.25)/60=0.2375��
//106+0.2375=106.2375��
//�������Ľ����106.2375��
//ͨ�����ת��֮���ҵ�λ�����꡾��γ30��40.8639'������104��5.7573'���Ϳ��Ա��
//��30.681065N,104.095955E�����������WGS-84�����꣩���ģ�������������GPS������λ��
//���ݹ��ʱ�׼����Ҫ������������GCJ-02ƫ��ת����ת���������ſ�����google��ͼ���ߵµ�ͼ��
//�Լ���Ѷ��ͼ�϶�λ������������ѭGCJ-02���ܣ�������ΪʲôҪ������������Ϊ���������ԭ��
//��ΪGCJ-02�ǲ������ת����
//����������׼ѰWGS-84��׼ת��ΪGCJ-02��C++Դ�룺
#include <math.h>  
const double pi = 3.14159265358979324;  
//  
// Krasovsky 1940  
//   
// a = 6378245.0, 1/f = 298.3   
// b = a * (1 - f)  
// ee = (a^2 - b^2) / a^2;   
const double a      = 6378245.0;  
const double ee     = 0.00669342162296594323;  

static BOOL outOfChina(double lat, double lon)   
{   
    if (lon < 72.004 || lon > 137.8347)  
        return TRUE;   
    if (lat < 0.8293 || lat > 55.8271)  
        return TRUE; 
    return FALSE;  
} 

static double transformLat(double x, double y)  
{   
    double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * sqrt(abs((int)x));  
    ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;   
    ret += (20.0 * sin(y * pi) + 40.0 * sin(y / 3.0 * pi)) * 2.0 / 3.0;   
    ret += (160.0 * sin(y / 12.0 * pi) + 320 * sin(y * pi / 30.0)) * 2.0 / 3.0;   
    return ret;  
}   

static double transformLon(double x, double y)  
{   
    double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(abs((int)x));   
    ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;   
    ret += (20.0 * sin(x * pi) + 40.0 * sin(x / 3.0 * pi)) * 2.0 / 3.0;  
    ret += (150.0 * sin(x / 12.0 * pi) + 300.0 * sin(x / 30.0 * pi)) * 2.0 / 3.0;   
    return ret;  
}   
/* 
���� 
wgLat:WGS-84γ��wgLon:WGS-84���� 
����ֵ�� 
mgLat��GCJ-02γ��mgLon��GCJ-02���� 
*/  
void gps_transform( float wgLat, float wgLon, float *mgLat, float *mgLon)   
{   
    if (outOfChina(wgLat, wgLon)) {   
        *mgLat = wgLat;  
        *mgLon = wgLon;  
        return;   
    }   
    double dLat = transformLat(wgLon - 105.0, wgLat - 35.0);   
    double dLon = transformLon(wgLon - 105.0, wgLat - 35.0);   
    double radLat = wgLat / 180.0 * pi; double magic = sin(radLat);  
    magic = 1 - ee * magic * magic; 
    double sqrtMagic = sqrt(magic);  
    dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * pi);  
    dLon = (dLon * 180.0) / (a / sqrtMagic * cos(radLat) * pi);  
    *mgLat = wgLat + dLat; 
    *mgLon = wgLon + dLon;  
} 

//�������ϵ�ת�������ǵ�����Ϳ�������ѭGCJ-02��׼�ĵ�ͼɾ��λ�ˣ�
//����������һ���ȽϺ��õĲ��Զ�λ����ַ��http://www.gpsspg.com/maps.htm
//�ٶȵ�ͼ��Щ���⣬����GCJ-02�������ֽ�����һ�μ��ܣ��ٶȰ�������ܱ�׼����BD-09��
//����������ܲ�û�ж��⹫�����������ܵ������Լ��������ܵ������ܻ��а취����������⡣
//�����Ҿ�����һ�����е�ת���취���������ԣ���λ��׼ȷ��
    
const double x_pi = 3.14159265358979324 * 3000.0 / 180.0;    
//�� GCJ-02 ����ת���� BD-09 ����  
void bd_encrypt(float gg_lat, float gg_lon, float *bd_lat, float *bd_lon)    
{ 
    double x = gg_lon, y = gg_lat;    
    double z = sqrt(x * x + y * y) + 0.00002 * sin(y * x_pi);    
    double theta = atan2(y, x) + 0.000003 * cos(x * x_pi);    
    *bd_lon = z * cos(theta) + 0.0065;    
    *bd_lat = z * sin(theta) + 0.006;    
}    
    
void bd_decrypt(float bd_lat, float bd_lon, float *gg_lat, float *gg_lon)    
{    
    double x = bd_lon - 0.0065, y = bd_lat - 0.006;    
    double z = sqrt(x * x + y * y) - 0.00002 * sin(y * x_pi);    
    double theta = atan2(y, x) - 0.000003 * cos(x * x_pi);    
    *gg_lon = z * cos(theta);    
    *gg_lat = z * sin(theta);    
} 

/***********************************************
����˵��:
����أ�
[GPS ��Ϣ ���ظ�ʽ]:
$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>*hh
<1> UTC ʱ�䣬 hhmmss.sss(ʱ����.����)��ʽ
<2> ��λ״̬�� A=��Ч��λ�� V=��Ч��λ
<3> γ�� ddmm.mmmm(�ȷ�)��ʽ(ǰ��� 0 Ҳ��������)
<4> γ�Ȱ��� N(������)�� S(�ϰ���)
<5> ���� dddmm.mmmm(�ȷ�)��ʽ(ǰ��� 0 Ҳ��������)
<6> ���Ȱ��� E(����)�� W(����)
<7> ��������(000.0~999.9 �ڣ�ǰ��� 0 Ҳ��������)
<8> ���溽��(000.0~359.9 �ȣ�������Ϊ�ο���׼��ǰ��� 0 Ҳ��������)
<9> UTC ���ڣ� ddmmyy(������)��ʽ
<10> ��ƫ��(000.0~180.0 �ȣ�ǰ��� 0 Ҳ��������)
<11> ��ƫ�Ƿ��� E(��)�� W(��)
<12> ģʽָʾ(�� NMEA0183 3.00 �汾����� A=������λ�� D=��֣� E=���㣬 N=������Ч)
���磺
��Ч���ݣ� $GPRMC,225530.000,A,3637.26040,N,11700.56340,E,0.000,97.17,220512,,,D*57
��Ч���ݣ� $GPRMC,000353.000,V,8960.0000,N,00000.0000,E,0.000,0.00,060180,,,N*4B
*/ 
void    GPS_infoUnpack(char *pe )  
{
    char    step    = 0;
    char    *pstr   = NULL;

    for ( step = 0; step < 11; step++, pe++ ) {                 //ȡ���ֶΡ�
        pstr    = pe;
        pe      = strstr(pstr,",");
        switch ( step ) {
            //ʱ��
        case 0:
            strncpy(Ctrl.Gps.StrInfo.UtcTime, pstr, sizeof(Ctrl.Gps.StrInfo.UtcTime));
            break;
            //״̬
        case 1:
            Ctrl.Gps.DecInfo.sta.GpsData = 1;
            strncpy(Ctrl.Gps.StrInfo.Av, pstr, sizeof(Ctrl.Gps.StrInfo.Av));
            //gps������Ч��ֱ�ӷ���
            if ( Ctrl.Gps.StrInfo.Av[0] != 'A' ) {
                Ctrl.Rec.GpsPos                 = 0;
                Ctrl.Gps.DecInfo.sta.GpsData    = 0;
                return;
            }
            break;
            //γ��
        case 2: {
            strncpy(Ctrl.Gps.StrInfo.Latitude, pstr, sizeof(Ctrl.Gps.StrInfo.Latitude));
            //���ɼ����ľ��Ⱥ�γ�ȸ�ʽ�ֱ���
            //γ��ddmm.mmmmm���ȷ֣�
            //��ʵ�������Ҫ�ڰٶȵ�ͼ��Google��ͼ����ʾ,����Ҫdd.mmmm��ddd.mmmm
            //������ת���㷨:
            //ת�����γ�� = ddmm.mmmm + (mm.mmmm/60)
            float   g       = atof(Ctrl.Gps.StrInfo.Latitude);
            int     d       = (int)g / 100;
            //ת�����γ�� = ddmm.mmmm + (mm.mmmm/60) 
            float   m       = g - d * 100; 
            g                = d + m / 60;
            Ctrl.Gps.DecInfo.Latitude   = g;
        } break;
        //�ϱ�����
        case 3:
            strncpy(Ctrl.Gps.StrInfo.LatitudeNS, pstr, sizeof(Ctrl.Gps.StrInfo.LatitudeNS));
            break;
            //����
        case 4: {
            strncpy(Ctrl.Gps.StrInfo.Longitude, pstr, sizeof(Ctrl.Gps.StrInfo.Longitude));
            //���ɼ����ľ��Ⱥ�γ�ȸ�ʽ�ֱ���
            //����dddmm.mmmmm���ȷ֣�
            //��ʵ�������Ҫ�ڰٶȵ�ͼ��Google��ͼ����ʾ,����Ҫdd.mmmm��ddd.mmmm
            //������ת���㷨:
            //ת����ľ��� = dddmm.mmmm + (mm.mmmm/60)
            float   g       = atof(Ctrl.Gps.StrInfo.Longitude);
            int     d       = (int)g / 100;
            float   m       = g - d * 100; 
            g                = d + m / 60;
            
            Ctrl.Gps.DecInfo.Longitude  = g; 
            
            Ctrl.Rec.Latitude   = (u32)(1000000 * Ctrl.Gps.DecInfo.Latitude);
            Ctrl.Rec.Longitude  = (u32)(1000000 * Ctrl.Gps.DecInfo.Longitude);
        } break;
        //��������
        case 5:
            strncpy(Ctrl.Gps.StrInfo.LongitudeEW, pstr,sizeof(Ctrl.Gps.StrInfo.LongitudeEW));
            
            //1������2���ϣ�3������4���ϣ�����δ֪
            if ( Ctrl.Gps.StrInfo.LongitudeEW[0] == 'E' ) {
                if ( Ctrl.Gps.StrInfo.LatitudeNS[0] == 'N' ) 
                    Ctrl.Rec.GpsPos   = 1;
                else
                    Ctrl.Rec.GpsPos   = 2;
            } else {
                if ( Ctrl.Gps.StrInfo.LatitudeNS[0] == 'N' ) 
                    Ctrl.Rec.GpsPos   = 3;
                else
                    Ctrl.Rec.GpsPos   = 4;
            }
            
            break;
            //
        case 6:
            strncpy(Ctrl.Gps.StrInfo.Speed, pstr, sizeof(Ctrl.Gps.StrInfo.Speed));
            Ctrl.Gps.DecInfo.Speed = (u16)atof(Ctrl.Gps.StrInfo.Speed);
            Ctrl.Rec.GpsSpeed    = (u16)(Ctrl.Gps.DecInfo.Speed * 1.852);
            break;
        case 7:
            strncpy(Ctrl.Gps.StrInfo.Course, pstr, sizeof(Ctrl.Gps.StrInfo.Course));
            break;
        case 8:
            strncpy(Ctrl.Gps.StrInfo.UtcDate, pstr, sizeof(Ctrl.Gps.StrInfo.UtcDate)); 
            if ( Ctrl.Gps.StrInfo.Av[0] == 'A' ) {
                time_s  t_tm;
                char    *strs    = Ctrl.Gps.StrInfo.UtcTime;
                char    str[4];
                strncpy(str, &strs[0],2);
                t_tm.tm_hour    = atoi(str);
                strncpy(str, &strs[2],2);
                t_tm.tm_min     = atoi(str);
                strncpy(str, &strs[4],2);
                t_tm.tm_sec     = atoi(str);
                strs            = Ctrl.Gps.StrInfo.UtcDate;
                strncpy(str, &strs[0],2);
                t_tm.tm_mday     = atoi(str);
                strncpy(str, &strs[2],2);
                t_tm.tm_mon     = atoi(str);
                strncpy(str, &strs[4],2);
                t_tm.tm_year    = atoi(str) + 2000;
                time_t  t;
                if ( t_tm.tm_hour < 16 ) {
                    t_tm.tm_hour   += 8;
                    t   = TIME_ConvCalendarToUnix(t_tm);
                } else {
                    t   = TIME_ConvCalendarToUnix(t_tm) + 8*3600;
                }
                //���ʱ����10����У׼һ��
                if ( abs(t - t_now) > 10 ) {
                    TIME_SetUnixTime(t);
                    tm_now  = TIME_GetCalendarTime();
                }
            }
            break;
        case 9:
            strncpy(Ctrl.Gps.StrInfo.MagneticDeg, pstr,sizeof(Ctrl.Gps.StrInfo.MagneticDeg));
            break;
        case 10:
            strncpy(Ctrl.Gps.StrInfo.MagneticDegEW, pstr, sizeof(Ctrl.Gps.StrInfo.MagneticDegEW));
            break;
        default:
            break;
        }
    }
}


/**************************************************************
* Description  : Q460����Э�顣
���ݽ����ɹ�������1�����򷵻�0��
* Author       : 2018/6/4 ����һ, by redmorningcn
*/
u8  Q460ProtocoUnpack(char *pstr,int len)
{ 
    char    *pe     = NULL;
    char    *div    = NULL;
    
    if(len < 255)
        pstr[len] = '\0';             //�ַ�������
    
    /**************************************************************
    * Description  : GPS��λ��Ϣ
    * Author       : 2018/6/4 ����һ, by redmorningcn
    */
    pe    = NULL;
    pe = strstr((const char *)pstr, (const char *)GPS_INFO_GET);        //ȷ���ǲ�ѯָ��
    
    if (pe ==  pstr) {
        /**************************************************************
        * Description  : �ϸ��жϡ�ֻ�����������������ַ��ڿ�ʼλ�����㡣
        ����ֹ����֡�г��������ض��ַ������
        * Author       : 2018/6/4 ����һ, by redmorningcn
        */
        Ctrl.Gps.DecInfo.sta.GpsComm    = 1;                //���յ�GPS��Ϣ��
        
        pe += (strlen(GPS_INFO_GET) + 1);                   //����ͷ
        GPS_infoUnpack(pe);                                 //ȡGPSλ����Ϣ
        
        return DEF_TRUE; 
    }
    
    /**************************************************************
    * Description  : GPS�ź�ǿ��
    * Author       : 2018/6/4 ����һ, by redmorningcn
    */
    pe    = NULL;
    pe = strstr((const char *)pstr, (const char *)GPS_RSSI_GET);//ȷ���ǲ�ѯָ��
    
    if (pe ==  pstr) {
        /**************************************************************
        * Description  : �ϸ��жϡ�ֻ�����������������ַ��ڿ�ʼλ�����㡣
        ����ֹ����֡�г��������ض��ַ������
        * Author       : 2018/6/4 ����һ, by redmorningcn
        */
        Ctrl.Gps.DecInfo.sta.GpsComm    = 1;                //���յ�GPS��Ϣ��
        
        pe += (strlen(GPS_RSSI_GET) );                      //����ͷ
        div = strstr(pe,"\r\n");
        if ( NULL != div ) {
            *div = '\0';
            Ctrl.Rec.Rssi = (u8)(atoi(pe));
        }        
        return DEF_TRUE; 
    }
    
    return DEF_FALSE;
}
    
 

/*******************************************************************************
* 				                end of file                                    *
*******************************************************************************/
#endif