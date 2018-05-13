#include <iostream>

using namespace std;
enum Error_code{success,underflow,overflow};
enum Plane_status{null,arriving,departing};//�ɻ�״̬
//enum Plane_emergency{mayday,normal};//�ɻ�����״̬����ʵ��
extern int maxqueue

/*
���ڷɻ�����״̬��
����ɻ����ڽ���״̬(mayday)����Ҫ�Ƚ����򽫻ὫĿǰ��������еķɻ��������У�
ʹ�ý���״̬�ķɻ����ڶ�ͷ������ж��mayday�ɻ������������У���ԭ�ж����еķɻ�����ԭ˳��
����������queue�С�
����״̬�ɻ��Ľ���������뱻���ܣ�
��ˣ���ʹԭ�ж�������������״̬�ɻ��Ի���ӣ����ͬʱ��ԭ�ж������һ�ܷɻ��ᱻ�������
*/
/*
���ܵ�����:
�ٶ���������ӵ�ж����ܵ��������ȸ���ͼ����ķɻ�ƥ��ȴ��ɻ������ٵ��ܵ�������mayday�ɻ�Ҳ����ˣ�
�˴���������Ҫ�Ƚϲ�ͬ�ܵ���queue��Ԫ�ص��������ҳ���Сֵ��
���ڶ��ܵ���������ʵ���ܵ�����Ҳ���ԣ�������������ܵ��¹ʻ��ƣ�
���ܵ����ܴ��ڲ�ͬԭ���¹رգ����ж����еķɻ�����ʱֹͣ�𽵡�
*/

class Random
{
 public:
     int possion(double rate) const;
     double uniform() const;
};

int Random::possion(double rate)//ʹ��inverse CDF������cumulative distribution function����possion�ֲ��ķ����������ͨ�����ȷֲ���������ó�possion�ֲ������
{

}

double Random::uniform()//0~1֮����ȷֲ���������������������0~1֮���������Ϊpossion�ֲ������ĸ���P(x=k)�������Ӧ��k��ֵ�����ο�C11���������ȷֲ�ģ��
{

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
protected:
    int count;
    int front,rear;
    T entry[maxqueue];
};

template<typename T> Extended_Queue<T>::Extended_Queue()
{
    //Extended_Queue* q=new Extended_Queue();
    count=0;
    front=0;
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
    int started() const;
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
    Plane_status=null;
}

Plane::Plane(int flt,int time,Plane_status)
{
    clock_start=time;
    flt_num=flt;
    state=status;
}

void Plane::land(int time)
{

}

enum Runway_activity{idle,land,takeoff};

class Runway
{
public:
    Runway(int limit);
    Error_code can_land(const Plane& current);
    Error_code can_takeoff(const Plane& current);
    Runway_activity activity(int time,Plane &moving);//�ܵ�״̬�������⣺��ξ����������ɣ���������ԭ�򣿵ȴ�ʱ����Сԭ��������ԭ�򣿣�
    void shut_down(int time) const;
private:
    Extended_Queue landing;//�ȴ�����ɻ�����
    Extended_Queue takeoff;//�ȴ���ɷɻ�����
    int queue_limit;//������󳤶�
    int num_land_requests;//Ҫ����ɻ���Ŀ
    int num_takeoff_requests;//Ҫ����ɷɻ���Ŀ
    int num_landing;//�ѽ���ɻ���Ŀ
    int num_takeoffs;//����ɷɻ���Ŀ
    int num_land_accepted;//�ڽ�������еķɻ���Ŀ
    int num_takeoff_accepted;//����ɶ����еķɻ���Ŀ
    int num_land_refused;//��ܾ�����ķɻ���Ŀ
    int num_takeoff_refused;//���ܾ���ɷɻ���Ŀ
    int land_wait;//�ɻ��ȴ�������ʱ��
    int takeoff_wait;//�ɻ��ȴ������ʱ��
    int idle_time;//�������ڿ���״̬��ʱ��
};

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
    <<"Total number of planes accepted for landing"
    << num_land_accepted<<endl
    <<"Total number of planes accepted for takeoff"
    << num_takeoff_accepted<<endl
    <<"Total number of planes refused for landing"
    << num_takeoff_accepted<<endl
    <<"Total number of planes refused for takeoff"
    << num_takeoff_accepted<<endl
    <<"Total number of planes that landed"
    << num_takeoff_accepted<<endl
    <<"Total number of planes that took off"
    << num_takeoff_accepted<<endl
    <<"Total number of planes left landing queue"
    << landing.count<<endl
    <<"Total number of planes left in takeoff queue"
    << takeoff.count<<endl;
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
    maxqueue=queue_limit;
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
        if (acceptable && arrival_rate . departure_rate > 1.0)
        cerr << "Safety Warning: This airport will become saturated. " << endl;
    }while(!acceptable);
}

int main()
{
    int end_time;
    int queue_limit;
    int flight_number=0;
    double arrival_rate,departure_rate;
    initialize(end_time,queue_limit,arrival_rate,departure_rate);
    Random variable;
    Runway small_airport(queue_limit);
    for (int current_time = 0; current_time<end_time;current_time++){
        int number_arrivals = variable.poisson(arrival_rate);//�ò��ɷֲ�д�����������λʱ���ڵ���ķɻ�����
        for (int i = 0; i < number_arrivals; i++){
            Plane current_plane(flight_number++, current_time, arriving);//mayday���ƴ��������˴�Ӧ��mayday�ֿ�
            if (small_airport.can_land(current_plane) != success)//can_land����Ϊ�жϷɻ��ܷ��亯������landing�����Ƿ�Ϊ������ִ����Ӵ���
            current_plane.refuse();//�ɻ����ܾ�����
        }
        int number_departures = variable.poisson(departure_rate);
        for (int j = 0; j<number_departures; j++){
            Plane current_plane(flight_number++, current_time, departing);
            if (small_airport.can_depart(current_plane) != success)//�жϷɻ��ܷ���ɣ���ִ����Ӵ���
            current_plane.refuse();//�ɻ����ܾ����
        }
        Plane moving_plane;
        switch (small_airport.activity(current_time,moving_plane)){//�����еķɻ����Ӳ�ִ�н������ɲ���
            case land:
                moving_plane.land(current_time);
                break;
            case takeoff:
                moving_plane.fly(current_time);
                break;
            case idle:
                run_idle(current_time);
        }
    }
 small_airport.shut_down(end_time);
 return 0;
}
