#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <cmath>
#include <algorithm>  

int C, R, F, N, B, T;

class Ride {
public:
	int a; // from x
	int b; // from y
	int x; // to x
	int y; // to y
	int s; // start time
	int f; // finish time
	
	int sc; //critical start time sc = f - distance(a, b, x, y)
	int n;  // number
	int length; // distance(a, b, x, y);
	bool isUsed;
};

int distance(int x1, int y1, int x2, int y2) {
	return std::abs(x1 - x2) + std::abs(y1 - y2);
}

class Car {
private:
	// data for fake travelings
	int _t, _x, _y;
	Ride *_ride = nullptr; // last ride for reverting
	bool isSaved;

	void Save(Ride *ride) {
		_x = x;
		_y = y;
		_t = t;
		_ride = ride;
	}
public:
	int t; // timeframe
	int x, y; // current x and y
	std::vector<int> path; // result path
	Car() : t(0), x(0), y(0) {};

	void Revert() {
		if (!_ride) {
			std::cout << "Revert without save. Error";
			return;
		}
		x = _x;
		y = _y;
		t = _t;
		path.pop_back();
		_ride->isUsed = false;
		_ride = nullptr;
	}

	void Perform(Ride &ride) {
		Save(&ride);
		t = std::max(t + distance(x, y, ride.a, ride.b), ride.s) + ride.length;
		x = ride.x;
		y = ride.y;
		path.push_back(ride.n);
		ride.isUsed = true;
	}
};

std::vector<Ride> rides;
std::vector<Car> cars;

void readInFile(char* name) {
	std::ifstream fileIn;
	fileIn.open(name, std::ifstream::in);
	fileIn >> R >> C >> F >> N >> B >> T;
	for (int i = 0; i < N; i++) {
		Ride ride;
		fileIn >> ride.a >> ride.b >> ride.x >> ride.y >> ride.s >> ride.f;
		rides.push_back(ride);
	}
	fileIn.close();
}

void initData() {
	std::vector<int> distances;
	for (int i = 0; i < N; i++) {
		rides[i].n = i;
		rides[i].sc = rides[i].f - distance(rides[i].a, rides[i].b, rides[i].x, rides[i].y);
		rides[i].length = distance(rides[i].a, rides[i].b, rides[i].x, rides[i].y);
		rides[i].isUsed = false;
	}

	for (int i = 0; i < F; i++) {
		Car car;
		cars.push_back(car);
	}
}

float calculateRate(Car car, Ride ride) {
	int road = distance(car.x, car.y, ride.a, ride.b);
	int timeOnStart = car.t + road;
	if (timeOnStart > ride.sc)
		return -1;
	int bonus = timeOnStart < ride.s ? B : 0;
	int timeToWait = std::max(ride.s - timeOnStart, 0);
	return ((1.0 * ride.length + bonus) / (road + ride.length + timeToWait));
}

class Recomendation {
public:
	Ride ride;
	float rate;
	Recomendation(Ride ride, float rate) : ride(ride), rate(rate){};
};

float calculateRateWithFeature(float currentRate, std::vector<Recomendation> futureRecomandations) {
	float result = currentRate;
	if (futureRecomandations.size() == 0)
		return currentRate;
	float k = 1 / futureRecomandations.size();
	for each (Recomendation recomandation in futureRecomandations)
	{
		result += recomandation.rate*k;
	}
	return result;
}

std::vector<Recomendation> getFewGoodRides(Car car, int count, bool isUseFeatureRanking = false) {
	std::vector<Recomendation> result;
	float worstRate = -1, currentRate;
	for each (Ride ride in rides)
	{
		if (ride.isUsed)
			continue;

		currentRate = calculateRate(car, ride);

		if (isUseFeatureRanking && currentRate + 1 > worstRate) {
			std::vector<Recomendation> possibleFeatureRides;
			car.Perform(ride);
			possibleFeatureRides = getFewGoodRides(car, 1);
			car.Revert();
			currentRate = calculateRateWithFeature(currentRate, possibleFeatureRides);
		}

		if (currentRate > worstRate) {
			result.push_back(Recomendation(ride, currentRate));
			std::sort(result.begin(), result.end(), [](Recomendation a, Recomendation b) {return a.rate > b.rate; });

			if (result.size() > count) {
				result.pop_back();
				worstRate = result.back().rate;
			}
		}
	}

	return result;
}

void process() {
	int count = rides.size() + 1;
	int next_count = count - 10;
	while (true) {
		std::sort(cars.begin(), cars.end(), [](Car a, Car b) {return a.t < b.t; });

		std::vector<Recomendation> recomendations = getFewGoodRides(cars[0], 1, cars[0].t + (R + C) / 2 < T);
		if (recomendations.size() == 0) {
			return;
		}

		cars[0].Perform(rides[recomendations[0].ride.n]);
		count--;		
		if (count < next_count) {
			std::cout << count << std::endl;
			next_count -= 10;
		}
	}
}

void writeOutFile(char* name) {
	std::ofstream file;
	file.open(name, std::ifstream::out);
	for each (Car car in cars)
	{
		file << car.path.size();
		for each (int idx in car.path)
		{
			file << ' ' << idx;
		}
		file << std::endl;
	}
	file.close();
}

int main(int argc, char* argv[]) {

	if (argc != 2) {
		std::cout << "Please specify 'in' file";
		return 0;
	}

	readInFile(argv[1]);
	initData();
	process();
	writeOutFile("result");

	return 0;
}