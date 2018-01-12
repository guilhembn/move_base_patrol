//
// Created by gbuisan on 11/8/17.
//

#include "ros/ros.h"
#include "tf/tf.h"
#include "signal.h"
#include "actionlib/client/simple_action_client.h"
#include "move_base_msgs/MoveBaseAction.h"

#define NODE_NAME "PatrolNav"
#define PATROL 1 //Goals are send forward and backward (0,1,2,1,0,1,...), either it cycles (0,1,2,0,1,2,0,...)


void sigintHandler(int sig){
    ROS_DEBUG_STREAM("node will now shutdown");
    // the default sigint handler, it calls shutdown() on node
    ros::shutdown();
    exit(sig);
}

int main(int argc, char** argv){
    std::vector<geometry_msgs::PoseStamped> goals;
    int current_goal = 1;
    int direction = 1;
    ros::Publisher publisher;

    ros::init(argc, argv, NODE_NAME);
    signal(SIGINT, sigintHandler);
    geometry_msgs::PoseStamped goal1, goal2, goal3;

    goal1.header.frame_id = "map";
    goal1.pose.position.x = 2.0;
    goal1.pose.position.y = 2.0;
    goal1.pose.orientation = tf::createQuaternionMsgFromYaw(-M_PI_2);

    goal2.header.frame_id = "map";
    goal2.pose.position.x = 1.5;
    goal2.pose.position.y = 16.0;
    goal2.pose.orientation = tf::createQuaternionMsgFromYaw(M_PI_2);

    goal3.header.frame_id = "map";
    goal3.pose.position.x = 8.0;
    goal3.pose.position.y = 14.0;
    goal3.pose.orientation = tf::createQuaternionMsgFromYaw(0);

    goals.push_back(goal1);
    goals.push_back(goal2);
    goals.push_back(goal3);

    actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> client("move_base", true);
    move_base_msgs::MoveBaseGoal goal;

    while (ros::ok()) {
        ros::spinOnce();
        client.waitForServer();
        goal.target_pose = goals[current_goal];
        client.sendGoal(goal);
#if PATROL
        if (current_goal == goals.size() - 1){
            direction = -1;
        } else if (current_goal == 0){
            direction = 1;
        }
#endif
        current_goal = (int)((current_goal + direction) % goals.size());
        client.waitForResult(ros::Duration(2.0));
        while (ros::ok() && client.getState() != actionlib::SimpleClientGoalState::SUCCEEDED){
            client.waitForResult(ros::Duration(2.0));
        }
    }
    return 0;

}
