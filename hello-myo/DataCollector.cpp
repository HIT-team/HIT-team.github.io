
#define _USE_MATH_DEFINES
#include <cmath>
#include "DataCollector.hpp"

DataCollector::DataCollector()
: onArm(false), _roll_w(0), _pitch_w(0), _yaw_w(0), 
  _roll_v(0), _pitch_v(0), _yaw_v(0), _scale(200), 
  _funct_on(true), _recalibrate(false), _current_pt(0), 
  _descale(10), currentPose()
{
	memset(_mean, 0, 3);
	memset(_min_max, 10, 3);
	memset(_position, 0, 3);
	memset(_velocity, 0, 3);
	memset(_pos_buff, 0, 9);
	memset(_vel_buff, 0, 9);
	memset(_acc_buff, 0, 9);
}
void DataCollector::onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
{
	using std::atan2;
	using std::asin;
	using std::sqrt;

	// Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
	float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
		               1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
	float pitch = asin(2.0f * (quat.w() * quat.y() - quat.z() * quat.x()));
	float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
		              1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));
	// Convert the floating point angles in radians to a scale from 0 to 20.
	_pos_buff[0][_current_pt] = static_cast<int>((roll + (float)M_PI) / (M_PI * 2.0f) * _scale);
	_pos_buff[1][_current_pt] = static_cast<int>((pitch + (float)M_PI / 2.0f) / M_PI * _scale);
	_pos_buff[2][_current_pt] = static_cast<int>((yaw + (float)M_PI) / (M_PI * 2.0f) * _scale);

	int temp[3];
	for (int i = 0; i < 3; i++) {
		temp[i] = _position[i];
		_position[i] = (averageInput(_pos_buff[i]) - _mean[i]);
		if (_position[i] > _scale / 2) {
			_position[i] = -1 * _scale / 2 + (_position[i] % (_scale / 2));
		}
		if (_position[i] > 0 && _position[i] > _min_max[i]) {
			_min_max[i] = _position[i];
		}
		else if (_position[i] < 0 && _position[i] < _min_max[i]) {
			_min_max[i] = -1 * _position[i];
		}
	}

	for (int i = 0; i < 3; i++) {
		_vel_buff[i][_current_pt] = _position[i] - temp[i];
		_velocity[i] = averageInput(_vel_buff[i]);
	}
	_current_pt = (_current_pt + 1) % TOTAL_AVG;
}

// onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
// making a fist, or not making a fist anymore.
void DataCollector::onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
{
	currentPose = pose;
	//processData(myo);
	_funct_on = true;
	if (_recalibrate) {
		_mean[0] = averageInput(_pos_buff[0]);
		_mean[1] = averageInput(_pos_buff[1]);
		_mean[2] = averageInput(_pos_buff[2]);
		std::cout << _mean[0] << ", " << _mean[1] << ", " << _mean[2] << "\n";
		_recalibrate = false;
	}
}

// onArmRecognized() is called whenever Myo has recognized a setup gesture after someone has put it on their
// arm. This lets Myo know which arm it's on and which way it's facing.
void DataCollector::onArmRecognized(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection)
{
	onArm = true;
	whichArm = arm;
	_recalibrate = true;
	for (int i = 0; i < 3; i++) {
		_position[i] = (_position[i] + _mean[i]) % _scale;
		_mean[i] = 0;
	}
}

// onArmLost() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
// it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
// when Myo is moved around on the arm.
void DataCollector::onArmLost(myo::Myo* myo, uint64_t timestamp)
{
	onArm = false;
	memset(_mean, 0, 3);
	for (int i = 0; i < 3; i++) {
		_position[i] = (_position[i] + _mean[i]) % _scale;
	}
}


// There are other virtual functions in DeviceListener that we could override here, like onAccelerometerData().
// For this example, the functions overridden above are sufficient.

void DataCollector::update()
{
	if (_funct_on) {
		processData();
	}
}

// We define this function to print the current values that were updated by the on...() functions above.
void DataCollector::print()
{

	// Clear the current line
	std::cout << '\r';

	// Print out the orientation. Orientation data is always available, even if no arm is currently recognized.
	//std::cout << '[' << std::string(roll_w, '*') << std::string(18 - roll_w, ' ') << ']'
	//          << '[' << std::string(pitch_w, '*') << std::string(18 - pitch_w, ' ') << ']'
	//          << '[' << std::string(yaw_w, '*') << std::string(18 - yaw_w, ' ') << ']';
	std::cout << _roll_w << "," << _pitch_w << "," << _yaw_w << ",";

	if (onArm) {
		// Print out the currently recognized pose and which arm Myo is being worn on.

		// Pose::toString() provides the human-readable name of a pose. We can also output a Pose directly to an
		// output stream (e.g. std::cout << currentPose;). In this case we want to get the pose name's length so
		// that we can fill the rest of the field with spaces below, so we obtain it as a string using toString().
		std::string poseString = currentPose.toString();

		//std::cout << '[' << (whichArm == myo::armLeft ? "L" : "R") << ']'
		//          << '[' << poseString << std::string(14 - poseString.size(), ' ') << ']';
		std::cout << (whichArm == myo::armLeft ? "0" : "1") << "," << poseString;
	}
	else {
		// Print out a placeholder for the arm and pose when Myo doesn't currently know which arm it's on.
		std::cout << "[?]" << '[' << std::string(14, ' ') << ']';
	}

	std::cout << std::flush;
}

int DataCollector::averageInput(int * in){
	int sum = 0;
	for (int i = 0; i < TOTAL_AVG; i++) {
		sum += in[i];
	}

	return sum / TOTAL_AVG;

}

void DataCollector::doCheese()// myo::Myo* myo)
{
	//I guess this hooks into the camera somehow
	std::cout << "COMMAND: cheese!" << std::endl;
	//myo->vibrate(myo::Myo::vibrationMedium);
	_funct_on = false;
}

std::string DataCollector::getLastPhoto()
{
	// FIXME: Change this to a proper find
	std::string fpath = "/home/clement/Documents/trollface.jpg";
	return fpath;
}

void DataCollector::doUpload()// myo::Myo* myo)
{
	//create command string for twurl
	// We'll assume we're only uploading the latest photo
	// A few bad things happening here.
	// 1. Assuming twurl is installed somewhere on the path
	// 2. Using system() to execute the command
	std::string cmd = "twurl -X POST \"/1.1/statuses/update_with_media.json\" --file \"" + getLastPhoto() + "\" --file-field \"media[]\" -d \"status=say cheese\"";
	//Fuckin' using system(), this is dumb.
	std::system(cmd.c_str());
	std::cout << "COMMAND: upload!" << std::endl;
	//myo->vibrate(myo::Myo::vibrationMedium);
	_funct_on = false;
}

void DataCollector::doDelete()//myo::Myo* myo)
{
	//delete last photo taken
	std::cout << "COMMAND: Exterminate!" << std::endl;
	//myo->vibrate(myo::Myo::vibrationMedium);
	std::string cmd = "rm -f " + getLastPhoto();
	_funct_on = false;
}

void DataCollector::doKawaii()//myo::Myo* myo)
{
	// Make last photo sooo kawaiii
	std::cout << "COMMAND: kawaii!" << std::endl;
	//myo->vibrate(myo::Myo::vibrationMedium);
	_funct_on = false;
}

void DataCollector::processData()//myo::Myo* myo)
{
	if ((currentPose == myo::Pose::fingersSpread) && _velocity[1] < -1)
	{
		doUpload();
	}
	else if ((currentPose == myo::Pose::fingersSpread) && _velocity[1] > 1)
	{
		doDelete();
	}
	else if ((currentPose == myo::Pose::fingersSpread || currentPose == myo::Pose::waveOut) && _velocity[2] < -2)
	{
		doKawaii();
	}
	else if ((currentPose == myo::Pose::thumbToPinky || currentPose == myo::Pose::fingersSpread) && _position[1] <= -40) {
		doCheese();
	}
}