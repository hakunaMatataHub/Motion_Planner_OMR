#include "global_planner.h"


GlobalPlanner:: GlobalPlanner()
{

	bGoalRecieved = false;
	bStartPoseRecieved = false;
	bPlan = false;

	startPose_sub = nh.subscribe("/mavros/local_position/pose", 5, &GlobalPlanner::startPose_callback,  this);
	goalPose_sub = nh.subscribe("/global_planner/goal", 5, &GlobalPlanner::goalPose_callback,  this);

	globalPath_pub = nh.advertise<planner_msgs::PointsArray>("/global_planner/global_path", 10, true);  


}

int GlobalPlanner::planGlobalPath(const Waypoint& startPose, const Waypoint& goalPose, std::vector<Waypoint>& globalPlannedPath)
{

     std::cout<<"yaw==========="<<goalPose.yw<<std::endl; 
	if(goalPose.yw ==1)
	{

		float distS2G = sqrt(pow((startPose.x - goalPose.x),2) + pow((startPose.y - goalPose.y),2) + pow((startPose.z - goalPose.z),2));
		float yw_diff = abs(startPose.yw - goalPose.yw);

		globalPlannedPath.push_back(startPose);
		int n =0;

		if(distS2G > MAX_PLANNING_DIST)
		{
			ROS_INFO("Goal too far !");
			return 0;
		}

		if (distS2G > 0.1 || yw_diff > 0.01) //minimum planning distance
		{

			float step = 5;

			n = distS2G/step;

			
			  	
			

			for(int i=1; i<n; i++)
			{
				Waypoint wp = Waypoint(startPose.x + (i*(goalPose.x-startPose.x))/n, startPose.y + (i*(goalPose.y-startPose.y))/n, startPose.x + (i*(goalPose.x-startPose.x))/n, startPose.yw + (i*(goalPose.yw-startPose.yw))/n);
				globalPlannedPath.push_back(wp);
			}

			globalPlannedPath.push_back(goalPose);
		}

		return n+1;   // no of waypoints in path including start and end waypoints

	}

	else
	{
		globalPlannedPath.push_back(startPose);
		//stright path
		for(int i=1; i<=4;i++)
		{
			Waypoint wp =Waypoint(30 + 5*i,30,30,0);
			globalPlannedPath.push_back(wp);
		}
		//circular path
		float theta =0;
		while(theta < 3.14)
		{
			Waypoint wp =Waypoint(30 + 20 *cos(theta),30 + 20 * sin(theta),30,3.14/2 + theta);
			globalPlannedPath.push_back(wp);
			theta = theta + 0.2;
		}
		return globalPlannedPath.size();
		
	}
	
} 

void GlobalPlanner::reset()
{
	globalPlannedPath.clear();
	bPlan = false;
	bStartPoseRecieved = false;

	planner_msgs::PointsArray PointsArrayMsg;

	PointsArrayMsg.bExecute = false;

	globalPath_pub.publish(PointsArrayMsg);
	//delete globalPath_pub;

	//globalPath_pub = nh.advertise<planner_msgs::PointsArray>("/global_planner/global_path", 10, true); 


}

void GlobalPlanner::startPose_callback(const geometry_msgs::PoseStampedConstPtr& msg )  // to do check message type pose from state estimator
{
	//std::cout<<"heyyyyyyyyyyyyyyyyyyy"<<std::endl;
	if(!bStartPoseRecieved)
	{
			startPose = Waypoint(msg->pose.position.x, msg->pose.position.y, msg->pose.position.y, tf::getYaw(msg->pose.orientation));  //to do include tf library
			bStartPoseRecieved = true;
			ROS_INFO("Start Pose Received .. Drone Localized !");

	}


}

void GlobalPlanner::goalPose_callback(const planner_msgs::Goal& msg)
{
	if(msg.bAbort && bGoalRecieved)
	{
		bGoalRecieved = false;
		ROS_INFO("Goal Dumped !");
		reset();
	}
	else if(!bGoalRecieved && !msg.bAbort)
	{
		goalPose = Waypoint(msg.x, msg.y, msg.z, msg.yw);
		bGoalRecieved = true;
		std::cout<<"yaw="<<goalPose.yw<<std::endl;
		ROS_INFO("Goal received !");
	}
	
}

void GlobalPlanner::MainLoop()
{
	ros::Rate loop_rate(25);
		//std::cout<<"heyyyyyyyyyyyyyyyyyyy"<<std::endl;


	while(ros::ok())
	{
		ros::spinOnce();

		if(/*bStartPoseRecieved ==*/ true)
		{
			if(/*bGoalRecieved ==true && */!bPlan)
			{
				//int n = planGlobalPath(startPose, goalPose, globalPlannedPath);
						//to fo vector arya to message conversion
				//if(n>2)                        //n is the number of waypoints
				if(1)
				{

					planner_msgs::PointsArray PointsArrayMsg;

					//for(int i=0; i<globalPlannedPath.size(); i++)
					{
						planner_msgs::Point PointMsg;
						PointMsg.x = 0;
						PointMsg.y = 0;
						PointMsg.yw = 0;
						PointMsg.z = 0;
						PointMsg.ndId =0;
						PointMsg.t =0;
						planner_msgs::PolyCoefficient coeff_msg;
						coeff_msg.c1 =0;
						coeff_msg.c2 =0;
						coeff_msg.c3 =0;
						coeff_msg.c4 =0;
						PointMsg.coeff.push_back(coeff_msg);
						PointMsg.coeff.push_back(coeff_msg);
						coeff_msg.c3 =1.5;						
						PointMsg.coeff.push_back(coeff_msg);
						coeff_msg.c3 =0;
						coeff_msg.c4 =0;
						PointMsg.coeff.push_back(coeff_msg);
						
						PointsArrayMsg.data.push_back(PointMsg); // wp 1 done
						PointMsg.coeff.clear();

						PointMsg.x = 0;
						PointMsg.y = 0;
						PointMsg.yw = 0;
						PointMsg.z = 3;
						PointMsg.ndId =1;
						PointMsg.t =2;
						//planner_msgs::PolyCoefficient coeff_msg;
						coeff_msg.c1 =0;
						coeff_msg.c2 =0;
						coeff_msg.c3 =0;
						coeff_msg.c4 =0;
						PointMsg.coeff.push_back(coeff_msg);
						PointMsg.coeff.push_back(coeff_msg);
						coeff_msg.c3 =1.5;						

						coeff_msg.c4 =3;						
						PointMsg.coeff.push_back(coeff_msg); //wp 2 done
						coeff_msg.c3 =0;
						coeff_msg.c4 =0;
						PointMsg.coeff.push_back(coeff_msg);

						PointsArrayMsg.data.push_back(PointMsg);
						PointMsg.coeff.clear();



						PointMsg.x = 0;
						PointMsg.y = 0;
						PointMsg.yw = 0;
						PointMsg.z =6 ;
						PointMsg.ndId =2;
						PointMsg.t =4;
					//	planner_msgs::PolyCoefficient coeff_msg;
						coeff_msg.c1 =0;
						coeff_msg.c2 =0;
						coeff_msg.c3 =0;
						coeff_msg.c4 =0;
						PointMsg.coeff.push_back(coeff_msg);
						PointMsg.coeff.push_back(coeff_msg);
						coeff_msg.c3 =1.5;	
						coeff_msg.c4 =6;						
						PointMsg.coeff.push_back(coeff_msg); //wp 3done
						coeff_msg.c3 =0;
						coeff_msg.c4 =0;
						PointMsg.coeff.push_back(coeff_msg);

						PointsArrayMsg.data.push_back(PointMsg);
						PointMsg.coeff.clear();



						PointMsg.x = 0;
						PointMsg.y = 0;
						PointMsg.yw = 0;
						PointMsg.z =9 ;
						PointMsg.ndId =3;
						PointMsg.t =6;
						//planner_msgs::PolyCoefficient coeff_msg;
						coeff_msg.c1 =0;
						coeff_msg.c2 =0;
						coeff_msg.c3 =0;
						coeff_msg.c4 =0;
						PointMsg.coeff.push_back(coeff_msg);
						PointMsg.coeff.push_back(coeff_msg);
						coeff_msg.c3 =0;	
						coeff_msg.c4 =9;						
						PointMsg.coeff.push_back(coeff_msg); //wp 4done
						coeff_msg.c3 =0;
						coeff_msg.c4 =0;
						PointMsg.coeff.push_back(coeff_msg);

						PointsArrayMsg.data.push_back(PointMsg);
						PointMsg.coeff.clear();

					}

					PointsArrayMsg.bExecute = true;
					globalPath_pub.publish(PointsArrayMsg);
					bPlan = true;
				}
				else
				{
					ROS_INFO ("Path Couldn't be found !");
					bGoalRecieved = false;
				}
			}
		}
		loop_rate.sleep();

	}


}

