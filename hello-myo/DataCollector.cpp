
#define _USE_MATH_DEFINES
#include <cmath>
#include "DataCollector.hpp"

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
	roll_w = static_cast<int>((roll + (float)M_PI) / (M_PI * 2.0f) * 18);
	pitch_w = static_cast<int>((pitch + (float)M_PI / 2.0f) / M_PI * 18);
	yaw_w = static_cast<int>((yaw + (float)M_PI) / (M_PI * 2.0f) * 18);
}

// onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
// making a fist, or not making a fist anymore.
void DataCollector::onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
{
	currentPose = pose;
	processData(myo);
}

// onArmRecognized() is called whenever Myo has recognized a setup gesture after someone has put it on their
// arm. This lets Myo know which arm it's on and which way it's facing.
void DataCollector::onArmRecognized(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection)
{
	onArm = true;
	whichArm = arm;
}

// onArmLost() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
// it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
// when Myo is moved around on the arm.
void DataCollector::onArmLost(myo::Myo* myo, uint64_t timestamp)
{
	onArm = false;
}


// There are other virtual functions in DeviceListener that we could override here, like onAccelerometerData().
// For this example, the functions overridden above are sufficient.

// We define this function to print the current values that were updated by the on...() functions above.
void DataCollector::print()
{
	// Clear the current line
	std::cout << '\r';

	// Print out the orientation. Orientation data is always available, even if no arm is currently recognized.
	//std::cout << '[' << std::string(roll_w, '*') << std::string(18 - roll_w, ' ') << ']'
	//          << '[' << std::string(pitch_w, '*') << std::string(18 - pitch_w, ' ') << ']'
	//          << '[' << std::string(yaw_w, '*') << std::string(18 - yaw_w, ' ') << ']';
	std::cout << roll_w << "," << pitch_w << "," << yaw_w << ",";

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

void DataCollector::doCheese(myo::Myo* myo)
{
	//I guess this hooks into the camera somehow
	std::cout << "COMMAND: cheese!" << std::endl;
	myo->vibrate(myo::Myo::vibrationMedium);
}

std::string getLastPhoto()
{
	// FIXME: Change this to a proper find
	std::string fpath = "/home/clement/Documents/trollface.jpg";
	return fpath;
}

void DataCollector::doUpload(myo::Myo* myo)
{
	//create command string for twurl
	// We'll assume we're only uploading the latest photo
	// A few bad things happening here.
	// 1. Assuming twurl is installed somewhere on the path
	// 2. Using system() to execute the command
	std::string cmd = "twurl -X POST \"/1.1/statuses/update_with_media.json\" --file \"" + 
		          getLastPhoto() + "\" --file-field \"media[]\"";
	//Fuckin' using system(), this is dumb.
	std::system(cmd);
	std::cout << "COMMAND: upload!" << std::endl;
	myo->vibrate(myo::Myo::vibrationMedium);
}

void DataCollector::doDelete(myo::Myo* myo)
{
	//delete last photo taken
	std::cout << "COMMAND: Exterminate!" << std::endl;
	myo->vibrate(myo::Myo::vibrationMedium);
	std::string cmd = 
}

void DataCollector::doKawaii(myo::Myo* myo)
{
	// Make last photo sooo kawaiii
	std::cout << "COMMAND: kawaii!" << std::endl;
	myo->vibrate(myo::Myo::vibrationMedium);
}

void DataCollector::processData(myo::Myo* myo)
{
	if (currentPose == myo::Pose::thumbToPinky && pitch_w <= 4) {
		doCheese(myo);
	}
	else if (currentPose == myo::Pose::fingersSpread && pitch_w <= 8)
	{
		doUpload(myo);
	}
	else if (currentPose == myo::Pose::fingersSpread && yaw_w >= 12)
	{
		doKawaii(myo);
	}
	else if (currentPose == myo::Pose::waveOut)
	{
		doDelete(myo);
	}
}
