#ifndef ROBOT_H
#define ROBOT_H

#include <ros/ros.h>
#include <std_msgs/Empty.h>
#include <std_msgs/Bool.h>
#include <std_msgs/String.h>
#include <std_srvs/Empty.h>
#include <string>
#include <sstream>
#include "elikos_roomba/movingobject.h"

static const double LOOP_RATE = 10.0;
// names (topics and services)
static const std::string ROBOTSTATE_TOPIC_NAME = "state";             // publishes current robot state
static const std::string ACTIVATE_SERVICE_NAME = "activate";          // service, activate robot
static const std::string DEACTIVATE_SERVICE_NAME = "deactivate";      // service, deactivate robot
static const std::string TOGGLEACT_SERVICE_NAME = "toggle_activate";  // service, toggle robot activation
static const std::string BUMPER_SERVICE_NAME = "bumper_trigger";      // exposed as feature for robot viz and such
// number parameters
static const int ROBOTSTATE_TOPIC_QUEUESIZE = 10;
// convention
static const double DEG_TO_RAD = 3.1415/180.0;          //[rad/deg]
static const double ROTATE_CCW = 1.0;                   // counterclockwise (positive angular.z)
static const double ROTATE_CW = -1.0;                   // clockwise (negative angular.z)
// speeds
static const double FORWARD_SPEED = 0.33;               //[m/s]
// physical dimensions
static const double DIAMETER = 0.3485;                  //[m] (according to specs)
static const double HEIGHT = 0.1;                       //[m] (according to specs)
static const double BUMPER_ANGLE = 180.0*DEG_TO_RAD;    //[deg] total angle interval (symmetrical) for bumper


class Robot : public MovingObject
{
    public:
        /*
         * Constructor
         * botType: std::string with type of robot ("ground" or "obstacle")
         */
        Robot(ros::NodeHandle& n, std::string botType, int r_id, tf::Vector3 initial_pos, double initial_yaw, std::string model_option);
        ~Robot();

        /*
         * Check if current robot is colliding with another robot and react accordingly
         */
        virtual void checkRobotCollision(tf::Vector3 pos) =0;

        /*
         * Check if quad is touching topswitch and react accordingly
         */
        virtual void checkTopInteraction(tf::Vector3 pos, double diameter) =0;

        /*
         * Update robot; called every spinOnce()
         */
        virtual void update();

        /*
         * ROS spin. Called only once (by node); contains ROS while loop
         */
        virtual void spin() =0;

        /*
         * Robot type accessor
         */
        std::string getRobotType() const;
    
    protected:
        double loop_hz_;
        bool is_running_slowly_;

        /* Current robot state message (String) */
        std_msgs::String robotState_msg_;

        /* Robot type */
        std::string robotType_;

        /* Robot id */
        int r_id_;

        /*===========================
         * Update
         *===========================*/
        /*
         * Update ground robot message based on current state
         */
        virtual void updateState() =0;

        /*
         * ROS spin once, called on every loop
         */
        virtual void spinOnce() =0;

        /*
         * Publish a specific CmdVel message (for testing purposes)
         */
        void publishCmdVel(geometry_msgs::Twist cmdVel_msg);

        /*
         * Publish robot state message (String)
         */
        void publishRobotState();

        /*===========================
         * Callbacks
         *===========================*/
        /*
         * Callback class method for robot activation service
         */
        bool activateCallback(std_srvs::Empty::Request& request, std_srvs::Empty::Response& response);

        /*
         * Callback class method for robot deactivation service
         */
        bool deactivateCallback(std_srvs::Empty::Request& request, std_srvs::Empty::Response& response);

        /*
         * Callback class method for robot toggle activate service
         */
        bool toglActivateCallback(std_srvs::Empty::Request& request, std_srvs::Empty::Response& response);

        /*===========================
         * Global state
         *===========================*/
        /*
         * Activate global robot state
         */
        virtual void activateRobot();

        /*
         * Deactivate global robot state
         */
        virtual void deactivateRobot();

        /*===========================
         * Other utilities
         *===========================*/
        /*
         * Get CmdVel message (Twist) from linear (x) velocity and angular (z) velocity
         */
        geometry_msgs::Twist getCmdVelMsg(float lin_x, float ang_z);

        /*
         * Wrapper for ROS_INFO_STREAM, includes robotType_ string and robot ID in message
         */
        void ROS_INFO_STREAM_ROBOT(std::string message);

    private:
        /*===========================
         * Publishers
         *===========================*/
        /* Robot state publisher */
        ros::Publisher robotState_pub_;

        /*===========================
         * Services
         *===========================*/
        /* Robot activation service  */
        ros::ServiceServer activate_srv_;
        /* Robot deactivation service */
        ros::ServiceServer deactivate_srv_;
        /* Robot toggle activate service */
        ros::ServiceServer toglActivate_srv_;

        /*
         * Get namespace from (robotType_ + r_id_)
         */
        std::string getRobotNamespace(std::string robotType, int robotId);
};

#endif  // ELIKOS_ROOMBA_ROBOT_H