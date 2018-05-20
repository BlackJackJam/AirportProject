#include <iostream>
#include <cmath>
#include <random>//Template of random
#include <time.h>//Use the clock to provide random numbers
#include <windows.h>
#define Strategy 0//�����𽵲���

using namespace std;
const int Maxqueue=10000;
enum Error_code{success,underflow,overflow};
enum Plane_status{null,arriving,departing,emergency};//�ɻ�״̬
//enum Plane_emergency{mayday,normal};//�ɻ�����״̬����ʵ��
//extern int maxqueue
/*��δʵ�֣�
���ڷɻ�����״̬��
����ɻ����ڽ���״̬(mayday)����Ҫ�Ƚ����򽫻ὫĿǰ��������еķɻ��������У�
ʹ�ý���״̬�ķɻ����ڶ�ͷ������ж��mayday�ɻ������������У���ԭ�ж����еķɻ�����ԭ˳��
����������queue�С�
����״̬�ɻ��Ľ���������뱻���ܣ�
��ˣ���ʹԭ�ж�������������״̬�ɻ��Ի���ӣ����ͬʱ��ԭ�ж������һ�ܷɻ��ᱻ�������
*/
/*��δʵ�֣�
���ܵ�����:
�ٶ���������ӵ�ж����ܵ��������ȸ���ͼ����ķɻ�ƥ��ȴ��ɻ������ٵ��ܵ�������mayday�ɻ�Ҳ����ˣ�
�˴���������Ҫ�Ƚϲ�ͬ�ܵ���queue��Ԫ�ص��������ҳ���Сֵ��
���ڶ��ܵ���������ʵ���ܵ�����Ҳ���ԣ�������������ܵ��¹ʻ��ƣ�
���ܵ����ܴ��ڲ�ͬԭ���¹رգ����ж����еķɻ�����ʱֹͣ�𽵡�
*/
/*����ʵ�֣�
�𽵲��ԣ�ͨ����������ѡ��ͬ���𽵿��Ʋ��ԣ�Ĭ��Ϊ�Ƚ��𽵶����еķɻ�����
�ɻ���Ŀ��Ķ�����ִ����Ӧ������
�������зɻ���Ŀ��ͬ������ִ�н�����С�
*/
/*
�����Ƚ����д���ɻ�����״̬��
�������зɻ������Ƚ��ĸ��ʹ̶�Ϊ0.0001����ʵ���������ɻ��Ƚ��ĸ��ʲ���̫�ߣ�
���۵�ǰ���ú��ֽ�����ԡ�ֻҪ�Ƚ��������зɻ����ͻ�ִ���Ƚ�����
*/
class Random
{
public:
     int poisson(double rate) const;
     double uniform() const;
private:
     int fac(int n) const;
};


int Random::poisson(double rate) const//ʹ��inverse CDF������cumulative distribution function����poisson�ֲ��ķ����������ͨ�����ȷֲ���������ó�possion�ֲ������
//ֻҪ����һ��0��1֮����������Ȼ�󿴲��ɷֲ���ǰ����͸պô����������������ˡ�(��ν��inverse CDF����)
{
    double u,p1;
    int i=0,s;
    u=uniform();
    p1=0;
    while(p1<u)
    {
        s=fac(i);//�׳�
        p1+=(exp(-rate)*pow(rate,i))/s;//�ӺͲ��ɷֲ�
        i++;
    }
    return i-1;
}
/*
�������ԣ������ɷֲ�ģ�⺯��������ֵ�������ϲ��ɷֲ�Ҫ�󣬵��ǲ�����Χ�ϴ���������ʵ��С��rate
*/

double Random::uniform() const//0~1֮����ȷֲ���������������������0~1֮���������Ϊpoisson�ֲ������ĸ���P(x=k)�������Ӧ��k��ֵ�����ο�C11���������ȷֲ�ģ��
{
    LARGE_INTEGER nFrequency;
    if(::QueryPerformanceFrequency(&nFrequency))
    {
            LARGE_INTEGER nStartCounter;
            ::QueryPerformanceCounter(&nStartCounter);
            std::default_random_engine random(nStartCounter.LowPart);
            std::uniform_real_distribution<double> dis2(0.0,1.0);
    return dis2(random);
    }//��ȡCPUʱ�䣬���Ծ�ȷ��΢�룬��˲��������������Ӽ���ʵ�����µ���ʱ������ͬ�����
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
    bool empty() const;//��Ա�ɵ��ò����޸�
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


//����Ϊ���в�������


class Plane
{
public:
    Plane();
    //Plane(int flt,int time,Plane_status status,Plane_emergency emerge);mayday���ܴ�ʵ��
    Plane(int flt,int time,Plane_status status);//flt��flight number
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
���ڼ�ʱ���ƣ�
��Ҫ���ɻ��ȴ�ʱ��Ӻͣ�������д�����and_time
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
    Runway_activity activity(int time,Plane &moving);//�ܵ�״̬�������⣺��ξ����������ɣ���������ԭ�򣿵ȴ�ʱ����Сԭ��������ԭ�򣿣�
    void shut_down(int time) const;
    friend void run_idle(int time);
private:
    Extended_Queue<Plane> landing;//�ȴ�����ɻ�����
    Extended_Queue<Plane> takeoff;//�ȴ���ɷɻ�����
    Extended_Queue<Plane> extreme_landing;//�Ƚ�����
    int queue_limit;//������󳤶�
    int num_land_requests;//Ҫ����ɻ���Ŀ
    int num_takeoff_requests;//Ҫ����ɷɻ���Ŀ
    int num_extreme_requests;//Ҫ���Ƚ��ɻ���
    int num_landings;//�ѽ���ɻ���Ŀ
    int num_takeoffs;//����ɷɻ���Ŀ
    int num_extremes;//���Ƚ��ɻ���
    int num_land_accepted;//�ڽ�������еķɻ���Ŀ
    int num_takeoff_accepted;//����ɶ����еķɻ���Ŀ
    int num_extreme_accepted;//�Ƚ������еķɻ���
    int num_land_refused;//��ܾ�����ķɻ���Ŀ
    int num_takeoff_refused;//���ܾ���ɷɻ���Ŀ
    int num_extreme_refused;//���ܾ��Ƚ��ɻ���Ŀ
    int land_wait;//�ɻ��ȴ�������ʱ��
    int takeoff_wait;//�ɻ��ȴ������ʱ��
    int extreme_wait;//�Ƚ��ɻ��ȴ�ʱ��
    int idle_time;//�������ڿ���״̬��ʱ��
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
���ɻ�����ȴ�����ʱ��ʼ��ʱ�����ɻ�������ʱ���ɻ�����ʱ������ύ���ܵ����Եȴ�ʱ����мӺ�
��һ��˼·����������ܵȴ�ʱ�䣬�ڵ�λʱ���ڣ����������ڵĳ�Ա�����м�����Ա�ȴ�ʱ��ͼӼ���ʱ�䵥λ
*/
void Runway::shut_down(int time) const
//�ܵ������������ܽ���㲢��ӡ����
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
        int number_extreme = variable.poisson(emergent_rate);//�ò��ɷֲ�д�����������λʱ���ڵ���ķɻ�����
        for (int i = 0; i < number_extreme; i++)
        {
            Plane current_plane(flight_number++, current_time, emergency);//mayday���ƴ������˴�Ӧ��mayday�ֿ�
            if (small_airport.can_extreme_landing(current_plane)!= success)//can_land����Ϊ�жϷɻ��ܷ��亯������landing�����Ƿ�Ϊ������ִ����Ӵ���
            current_plane.refuse();//�ɻ����ܾ�����
        }
        int number_arrivals = variable.poisson(arrival_rate);//�ò��ɷֲ�д�����������λʱ���ڵ���ķɻ�����
        for (int i = 0; i < number_arrivals; i++)
        {
            Plane current_plane(flight_number++, current_time, arriving);//mayday���ƴ������˴�Ӧ��mayday�ֿ�
            if (small_airport.can_land(current_plane)!= success)//can_land����Ϊ�жϷɻ��ܷ��亯������landing�����Ƿ�Ϊ������ִ����Ӵ���
            current_plane.refuse();//�ɻ����ܾ�����
        }
        int number_departures = variable.poisson(departure_rate);
        for (int j = 0; j<number_departures; j++)
        {
            Plane current_plane(flight_number++, current_time, departing);
            if (small_airport.can_takeoff(current_plane)!= success)//�жϷɻ��ܷ���ɣ���ִ����Ӵ���
            current_plane.refuse();//�ɻ����ܾ����
        }
        Plane moving_plane;
        switch (small_airport.activity(current_time,moving_plane))
        {//�����еķɻ����Ӳ�ִ�н������ɲ���
            case land:
                moving_plane.land(current_time);//��ӡ��ǰʱ�䵥Ԫ���зɻ�����
                break;
            case takeoff:
                moving_plane.fly(current_time);//��ӡ��ǰʱ�䵥Ԫ���зɻ����
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

