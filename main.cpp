#include <iostream>
#include <cmath>
#include <random>//Template of random
#include <time.h>//Use the clock to provide random numbers
#include <windows.h>
#define Strategy 0//控制起降策略

using namespace std;
const int Maxqueue=10000;
enum Error_code{success,underflow,overflow};
enum Plane_status{null,arriving,departing,emergency};//飞机状态
//enum Plane_emergency{mayday,normal};//飞机紧急状态，待实现
//extern int maxqueue
/*（未实现）
关于飞机紧急状态：
如果飞机处于紧急状态(mayday)，需要迫降，则将会将目前降落队列中的飞机重新排列，
使得紧急状态的飞机处于队头（如果有多架mayday飞机，则依次排列），原有队列中的飞机按照原顺序，
重新排列于queue中。
紧急状态飞机的降落请求必须被接受，
因此，即使原有队列已满，紧急状态飞机仍会入队，与此同时，原有队列最后一架飞机会被逐出队列
*/
/*（未实现）
多跑道机制:
假定机场可以拥有多条跑道，则优先给企图降落的飞机匹配等待飞机数最少的跑道（对于mayday飞机也是如此）
此处，我们需要比较不同跑道的queue中元素的数量，找出最小值。
对于多跑道机场（其实单跑道机场也可以），还可以设计跑道事故机制，
即跑道可能处于不同原因导致关闭，所有队列中的飞机均暂时停止起降。
*/
/*（已实现）
起降策略：通过条件编译选择不同的起降控制策略，默认为比较起降队列中的飞机数，
飞机数目多的队列先执行相应操作。
若两队列飞机数目相同，优先执行降落队列。
*/
/*
现以迫降队列代替飞机紧急状态。
将机场有飞机请求迫降的概率固定为0.0001（现实机场遇到飞机迫降的概率不会太高）
不论当前采用何种降落策略。只要迫降队列中有飞机，就会执行迫降队列
*/
class Random
{
public:
     int poisson(double rate) const;
     double uniform() const;
private:
     int fac(int n) const;
};


int Random::poisson(double rate) const//使用inverse CDF方法（cumulative distribution function）将poisson分布的反函数求出，通过均匀分布随机数，得出possion分布随机数
//只要生成一个0到1之间的随机数，然后看泊松分布的前几项和刚好大于这个随机数就行了。(所谓的inverse CDF方法)
{
    double u,p1;
    int i=0,s;
    u=uniform();
    p1=0;
    while(p1<u)
    {
        s=fac(i);//阶乘
        p1+=(exp(-rate)*pow(rate,i))/s;//加和泊松分布
        i++;
    }
    return i-1;
}
/*
经过测试，本泊松分布模拟函数产生数值基本符合泊松分布要求，但是波动范围较大，且倾向于实际小于rate
*/

double Random::uniform() const//0~1之间均匀分布随机函数，均匀随机生成0~1之间的数（作为poisson分布函数的概率P(x=k)，反求对应的k数值），参考C11浮点数均匀分布模板
{
    LARGE_INTEGER nFrequency;
    if(::QueryPerformanceFrequency(&nFrequency))
    {
            LARGE_INTEGER nStartCounter;
            ::QueryPerformanceCounter(&nStartCounter);
            std::default_random_engine random(nStartCounter.LowPart);
            std::uniform_real_distribution<double> dis2(0.0,1.0);
    return dis2(random);
    }//调取CPU时间，可以精确到微秒，因此不用重新制作种子即可实现重新调用时产生不同随机数
}

int Random::fac(int n) const
{
    if(n=0) return 1;
    else
    {
        int k,n1=1;
    for (k=1;k<=n;k++)
    {
        n1*=k;
    }
    return n1;
    }
}

template<typename T> class Extended_Queue
{
public:
    Extended_Queue();
    bool empty() const;//成员可调用不可修改
    bool full() const;
    int size() const;
    Error_code serve();
    Error_code append(const T &item);
    Error_code retrieve(T &item) const;
    void clear();
    Error_code serve_and_retrieve(T &item);
    void setlimit(int limit);
    friend class Runway;
protected:
    int maxqueue;
    int count;
    int front,rear;
    T entry[Maxqueue];
};

template<typename T> Extended_Queue<T>::Extended_Queue()
{
    //Extended_Queue* q=new Extended_Queue();
    count=0;
    front=0;
    maxqueue=Maxqueue;
    rear=maxqueue-1;
}


template<typename T> void Extended_Queue<T>::setlimit(int limit)
{
    //Extended_Queue* q=new Extended_Queue();
    maxqueue=limit;
    rear=maxqueue-1;
}

template<typename T> bool Extended_Queue<T>::empty() const
{
    if (count>0) return false;
    return true;
}

template<typename T> int Extended_Queue<T>::size() const
{
    return count;
}

template<typename T> bool Extended_Queue<T>::full() const
{
    int i;
    if (count==maxqueue) return true;
    return false;
}

template<typename T> Error_code Extended_Queue<T>::append(const T &item)
{
if (count>=maxqueue) return overflow;
count++;
rear=((rear+1)==maxqueue)?0:rear+1;
entry[rear]=item;
return success;
}

template<typename T> Error_code Extended_Queue<T>::serve()
{
if (empty()) return underflow;
count--;
front=((front+1)==maxqueue)?0:front+1;
return success;
}

template<typename T> Error_code Extended_Queue<T>::retrieve(T &item) const
{
if (empty()) return underflow;
item=entry[front];
return success;
}

template<typename T> void Extended_Queue<T>::clear()
{
    while(count>0)
    {
        serve();
    }
}

template<typename T> Error_code Extended_Queue<T>::serve_and_retrieve(T &item)
{
if (empty()) return underflow;
item=entry[front];
count--;
front=((front+1)==maxqueue)?0:front+1;
return success;
}


//以上为队列操作函数


class Plane
{
public:
    Plane();
    //Plane(int flt,int time,Plane_status status,Plane_emergency emerge);mayday功能待实现
    Plane(int flt,int time,Plane_status status);//flt即flight number
    void refuse() const;//takeoff or landing is refused
    void land(int time) const;//if landing
    void fly(int time) const;
    void emergency_land(int time) const;
    int started() const;
    friend class Runway;
private:
    int flt_num;//flight number
    int clock_start;//count waiting time
    Plane_status state;
    //Plane_emergency emerging;//under construction
};
/*
关于计时机制：
需要将飞机等待时间加和，详见下列代码中and_time
*/

Plane::Plane()
{
    clock_start=0;
    flt_num=0;
    state=null;
}

Plane::Plane(int flt,int time,Plane_status status)
{
    clock_start=time;
    flt_num=flt;
    state=status;
}

void Plane::refuse() const
{
	if(state==arriving)
    {
        cout.setf(std::ios::right);
        cout.width(7);
        cout<<" ";
        cout.setf(std::ios::left);
        cout.width(8);
        cout<<"Plane number "<<flt_num<<" told try to land again later."<<endl;
    }
	if(state==departing)
    {
        cout.setf(std::ios::right);
        cout.width(7);
        cout<<" ";
        cout.setf(std::ios::left);
        cout.width(8);
        cout<<"Plane number "<<flt_num<<" told try to take off again later."<<endl;
    }
    if(state==emergency)
    {
        cout.setf(std::ios::right);
        cout.width(7);
        cout<<" ";
        cout.setf(std::ios::left);
        cout.width(8);
        cout<<"Plane number "<<flt_num<<" told try to take an emergency landing again later."<<endl;
    }
}

void Plane::land(int time) const
{
    int wait_time=time-clock_start;
    cout.setf(std::ios::right);
    cout.width(6);
    cout<<time<<":";
    cout.setf(std::ios::left);
    cout.width(8);
    cout<<"Plane number "<<flt_num<<" land after "<<wait_time<<" time units"<<" in the landing queue."<<endl;
}

void Plane::emergency_land(int time) const
{
    int wait_time=time-clock_start;
    cout.setf(std::ios::right);
    cout.width(6);
    cout<<time<<":";
    cout.setf(std::ios::left);
    cout.width(8);
    cout<<"Plane number "<<flt_num<<" take an emergency landing successfully after "<<wait_time<<" time units"<<" in the landing queue."<<endl;
}

void Plane::fly(int time) const
{
	int wait_time=time-clock_start;
	cout.setf(std::ios::right);
    cout.width(6);
    cout<<time<<":";
    cout.setf(std::ios::left);
    cout.width(8);
    cout<<"Plane number "<<flt_num<<" take off after "<<wait_time<<" time units"<<" in the takeoff queue."<<endl;
}

int Plane::started() const
{
	return clock_start;
}

enum Runway_activity{idle,land,takeoff,emergency_landing};

class Runway
{
public:
    Runway(int limit);
    Error_code can_land(const Plane& current);
    Error_code can_takeoff(const Plane& current);
    Error_code can_extreme_landing(const Plane& current);
    Runway_activity activity(int time,Plane &moving);//跑道状态。（问题：如何决定降落或起飞？降落优先原则？等待时间最小原则？先来后到原则？）
    void shut_down(int time) const;
    friend void run_idle(int time);
private:
    Extended_Queue<Plane> landing;//等待降落飞机队列
    Extended_Queue<Plane> takeoff;//等待起飞飞机队列
    Extended_Queue<Plane> extreme_landing;//迫降队列
    int queue_limit;//队列最大长度
    int num_land_requests;//要求降落飞机数目
    int num_takeoff_requests;//要求起飞飞机数目
    int num_extreme_requests;//要求迫降飞机数
    int num_landings;//已降落飞机数目
    int num_takeoffs;//已起飞飞机数目
    int num_extremes;//已迫降飞机数
    int num_land_accepted;//在降落队列中的飞机数目
    int num_takeoff_accepted;//在起飞队列中的飞机数目
    int num_extreme_accepted;//迫降队列中的飞机数
    int num_land_refused;//别拒绝降落的飞机数目
    int num_takeoff_refused;//被拒绝起飞飞机数目
    int num_extreme_refused;//被拒绝迫降飞机数目
    int land_wait;//飞机等待降落总时间
    int takeoff_wait;//飞机等待起飞总时间
    int extreme_wait;//迫降飞机等待时间
    int idle_time;//机场处于空闲状态总时间
};

Runway::Runway(int limit)
{
    queue_limit=limit;
    landing.setlimit(queue_limit);
    takeoff.setlimit(queue_limit);
    extreme_landing.setlimit(queue_limit);
    num_land_requests=0;
    num_takeoff_requests=0;
    num_extreme_requests=0;
    num_landings=0;
    num_takeoffs=0;
    num_extremes=0;
    num_land_accepted=0;
    num_takeoff_accepted=0;
    num_extreme_accepted=0;
    num_land_refused=0;
    num_takeoff_refused=0;
    num_extreme_refused=0;
    land_wait=0;
    takeoff_wait=0;
    extreme_wait=0;
    idle_time=0;
}

Error_code Runway::can_land(const Plane& current)
{
    num_land_requests++;
    cout.setf(std::ios::right);
    cout.width(7);
    cout<<" ";
    cout.setf(std::ios::left);
    cout.width(8);
    cout<<"Plane number "<<current.flt_num<<" ready to land."<<endl;
    if (landing.append(current)==success)
    {
        num_land_accepted++;
        return success;
    }
    else
    {
        num_land_refused++;
        return overflow;
    }
}

Error_code Runway::can_takeoff(const Plane& current)
{
    num_takeoff_requests++;
    cout.setf(std::ios::right);
    cout.width(7);
    cout<<" ";
    cout.setf(std::ios::left);
    cout.width(8);
    cout<<"Plane number "<<current.flt_num<<" ready to take off."<<endl;
    if (takeoff.append(current)==success)
    {
        num_takeoff_accepted++;
        return success;
    }
    else
    {
        num_takeoff_refused++;
        return overflow;
    }
}

Error_code Runway::can_extreme_landing(const Plane& current)
{
    num_extreme_requests++;
    cout.setf(std::ios::right);
    cout.width(7);
    cout<<" ";
    cout.setf(std::ios::left);
    cout.width(8);
    cout<<"Plane number "<<current.flt_num<<" ready to take an emergency landing."<<endl;
    if (extreme_landing.append(current)==success)
    {
        num_extreme_accepted++;
        return success;
    }
    else
    {
        num_extreme_refused++;
        return overflow;
    }
}

#if Strategy
Runway_activity Runway::activity(int time,Plane &moving)
{
    if (extreme_landing.count>0)
    {
        extreme_landing.serve_and_retrieve(moving);
        land_wait+=landing.count;
        takeoff_wait+=takeoff.count;
        extreme_wait+=extreme_landing.count;
        num_extremes++;
        return emergency_landing;
    }
    else
    {
        if ((landing.count==0)&&(takeoff.count==0))
        {
            idle_time++;
            return idle;
        }
        else
        {
            if (landing.count>=takeoff.count)
            {
                landing.serve_and_retrieve(moving);
                land_wait+=landing.count;
                takeoff_wait+=takeoff.count;
                num_landings++;
                return land;
            }
            if (landing.count<takeoff.count)
            {
                takeoff.serve_and_retrieve(moving);
                land_wait+=landing.count;
                takeoff_wait+=takeoff.count;
                num_takeoffs++;
                return Runway_activity::takeoff;
            }
        }
    }
}
#else
Runway_activity Runway::activity(int time,Plane &moving)
{
    if(extreme_landing.count>0)
    {
        extreme_landing.serve_and_retrieve(moving);
        land_wait+=landing.count;
        takeoff_wait+=takeoff.count;
        extreme_wait+=extreme_landing.count;
        num_extremes++;
        return emergency_landing;
    }
    else
    {
     if ((landing.count==0)&&(takeoff.count==0))
    {
        idle_time++;
        return idle;
    }
    else
    {
    if (landing.count>0)
    {
        landing.serve_and_retrieve(moving);
        land_wait+=landing.count;
        takeoff_wait+=takeoff.count;
        num_landings++;
        return land;
    }
    else
    {
        takeoff.serve_and_retrieve(moving);
        land_wait+=landing.count;
        takeoff_wait+=takeoff.count;
        num_takeoffs++;
        return Runway_activity::takeoff;
    }
    }
    }
}

#endif

void run_idle(int &current_time)
{
    cout.setf(std::ios::right);
    cout.width(6);
    cout<<current_time<<":";
    cout.setf(std::ios::left);
    cout.width(8);
    cout<<"Runway is idle."<<endl;
}

/*
当飞机进入等待队列时开始计时，当飞机可以起降时，飞机将计时器结果提交给跑道，对等待时间进行加和
另一种思路：对于求解总等待时间，在单位时间内，检索队列内的成员数，有几个成员等待时间就加几个时间单位
*/
void Runway::shut_down(int time) const
//跑道数据在这里总结计算并打印出来
 {
    cout<< "Simulation has concluded after"<<time<<"time units"<<endl
    <<"Total number of planes processed"
    <<(num_land_requests+num_takeoff_requests)<<endl
    <<"Total number of planes asking to land"
    << num_land_requests<<endl
    <<"Total number of planes asking to taking off"
    << num_takeoff_requests<<endl
    <<"Total number of planes asking to take emergency landing"
    << num_extreme_requests<<endl
    <<"Total number of planes accepted for landing"
    << num_land_accepted<<endl
    <<"Total number of planes accepted for takeoff"
    << num_takeoff_accepted<<endl
    <<"Total number of planes accepted for emergency landing"
    << num_extreme_accepted<<endl
    <<"Total number of planes refused for landing"
    << num_land_refused<<endl
    <<"Total number of planes refused for takeoff"
    << num_takeoff_refused<<endl
    <<"Total number of planes refused for emergency landing"
    << num_extreme_refused<<endl
    <<"Total number of planes that landed"
    << num_landings<<endl
    <<"Total number of planes that took off"
    << num_takeoffs<<endl
    <<"Total number of planes that took emergency landing"
    << num_extremes<<endl
    <<"Total number of planes left landing queue"
    << landing.count<<endl
    <<"Total number of planes left in takeoff queue"
    << takeoff.count<<endl
    <<"Total number of planes left in emergency landing queue"
    << extreme_landing.count<<endl;
    cout << "Percentage of time runway idle "
    << 100.0 * ((float) idle_time)/((float) time) << "%" << endl;
    cout << "Average wait in landing queue "
    << ((float) land_wait)/((float) num_landings) << " time units";
    cout << endl << "Average wait in takeoff queue "
    << ((float) takeoff_wait)/((float) num_takeoffs)
    << " time units" << endl;
    cout << "Average observed rate of planes wanting to land "
    << ((float) num_land_requests)/((float) time)
    << " per time unit" << endl;
    cout << "Average observed rate of planes wanting to take off "
    << ((float) num_takeoff_requests)/((float) time)
    << " per time unit" << endl;
 }


void initialize(int &end_time,int &queue_limit,double &arrival_rate,double &departure_rate)
{
    cout<<"This program simulates an airport with only one runway."<<endl
    <<"one plane can land or depart in each unit of time."<<endl;
    cout<<"Up to what number of planes can be waiting to land or take off at any time?"<<flush;
    cin>>queue_limit;
    cout<<"How many units of time will the simulation run?"<<flush;
    cin>>end_time;
    bool acceptable;
    do
    {
        cout << "Expected number of arrivals per unit time?" << flush;
        cin >> arrival_rate;
        cout << "Expected number of departures per unit time?" << flush;
        cin >> departure_rate;
        if (arrival_rate < 0.0 || departure_rate < 0.0)
        cerr << "These rates must be nonnegative." << endl;
        else
        acceptable = true;
        if (acceptable && arrival_rate + departure_rate > 1.0)
        cerr << "Safety Warning: This airport will become saturated. " << endl;
    }while(!acceptable);
}

int main()
{
    int end_time;
    int queue_limit;
    int flight_number=0;
    double arrival_rate,departure_rate,emergent_rate=0.0001;
    initialize(end_time,queue_limit,arrival_rate,departure_rate);
    Random variable;
    Runway small_airport(queue_limit);
    for (int current_time = 0; current_time<end_time;current_time++)
        {
        int number_extreme = variable.poisson(emergent_rate);//用泊松分布写随机函数（单位时间内到达的飞机数）
        for (int i = 0; i < number_extreme; i++)
        {
            Plane current_plane(flight_number++, current_time, emergency);//mayday机制待处理，此处应将mayday分开
            if (small_airport.can_extreme_landing(current_plane)!= success)//can_land函数为判断飞机能否降落函数，即landing队列是否为满，并执行入队处理
            current_plane.refuse();//飞机被拒绝降落
        }
        int number_arrivals = variable.poisson(arrival_rate);//用泊松分布写随机函数（单位时间内到达的飞机数）
        for (int i = 0; i < number_arrivals; i++)
        {
            Plane current_plane(flight_number++, current_time, arriving);//mayday机制待处理，此处应将mayday分开
            if (small_airport.can_land(current_plane)!= success)//can_land函数为判断飞机能否降落函数，即landing队列是否为满，并执行入队处理
            current_plane.refuse();//飞机被拒绝降落
        }
        int number_departures = variable.poisson(departure_rate);
        for (int j = 0; j<number_departures; j++)
        {
            Plane current_plane(flight_number++, current_time, departing);
            if (small_airport.can_takeoff(current_plane)!= success)//判断飞机能否起飞，并执行入队处理
            current_plane.refuse();//飞机被拒绝起飞
        }
        Plane moving_plane;
        switch (small_airport.activity(current_time,moving_plane))
        {//队列中的飞机出队并执行降落或起飞操作
            case land:
                moving_plane.land(current_time);//打印当前时间单元内有飞机降落
                break;
            case takeoff:
                moving_plane.fly(current_time);//打印当前时间单元内有飞机起飞
                break;
            case emergency_landing:
                moving_plane.emergency_land(current_time);
                break;
            case idle:
                run_idle(current_time);
        }
    }
 small_airport.shut_down(end_time);
 return 0;
}

