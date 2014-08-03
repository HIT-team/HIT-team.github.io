#ifndef __DATACOLLECTOR_H__
#define __DATACOLLECTOR_H__

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>

// The only file that needs to be included to use the Myo C++ SDK is myo.hpp.
#include <myo/myo.hpp>

static const int TOTAL_AVG = 5;

class DataCollector : public myo::DeviceListener {
public:
	DataCollector();
	// onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
	// as a unit quaternion.
	void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat);

	// onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
	// making a fist, or not making a fist anymore.
	void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose);

	// onArmRecognized() is called whenever Myo has recognized a setup gesture after someone has put it on their
	// arm. This lets Myo know which arm it's on and which way it's facing.
	void onArmRecognized(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection);

	// onArmLost() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
	// it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
	// when Myo is moved around on the arm.
	void onArmLost(myo::Myo* myo, uint64_t timestamp);

	// There are other virtual functions in DeviceListener that we could override here, like onAccelerometerData().
	// For this example, the functions overridden above are sufficient.

	// We define this function to print the current values that were updated by the on...() functions above.
	void print();
	void update();

private:
	// These values are set by onArmRecognized() and onArmLost() above.
	bool onArm;
	myo::Arm whichArm;

	// These values are set by onOrientationData() and onPose() above.
	int _roll_w, _pitch_w, _yaw_w;
	int _roll_v, _pitch_v, _yaw_v;
	int _position[3];
	int _velocity[3];
	int _scale, _descale;
	int _pos_buff[3][TOTAL_AVG];
	int _vel_buff[3][TOTAL_AVG];
	int _acc_buff[3][TOTAL_AVG];
	int _mean[3];
	int _min_max[3];
	int _current_pt;

	bool _recalibrate;
	bool _funct_on;
	myo::Pose currentPose;

	int averageInput(int * in);
	void doCheese();// myo::Myo* myo);
	std::string getLastPhoto();
	void doUpload();// myo::Myo* myo);
	void doDelete();// myo::Myo* myo);
	void doKawaii();// myo::Myo* myo);
	void processData();// myo::Myo* myo);

};

#endif
