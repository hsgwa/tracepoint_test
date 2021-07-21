#include <chrono>
#include <memory>
#include <iostream>
#include <utility>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"

using namespace std::chrono_literals;

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  auto topic_name = "chatter";


  auto talker_node1 = std::make_shared<rclcpp::Node>
    ("talker", rclcpp::NodeOptions().use_intra_process_comms(true));


  auto talker_pub1 = talker_node1->create_publisher<std_msgs::msg::Int32>(topic_name, 1);
  auto publish1 = [&talker_pub1](){
                   static int n = 0;
                   // auto msg = std::make_unique<std_msgs::msg::Int32>();
                   std_msgs::msg::Int32 msg;
                   msg.data = n++;
                   std::cout << "message publish " << msg.data << std::endl;
                   talker_pub1->publish (msg);
                 };
  auto talker_timer1 = talker_node1->create_wall_timer(1s, publish1);

  auto talker_node2 = std::make_shared<rclcpp::Node>
    ("talker", rclcpp::NodeOptions().use_intra_process_comms(true));

  auto talker_pub2 = talker_node2->create_publisher<std_msgs::msg::Int32>(topic_name, 1);
  auto publish2 = [&talker_pub2](){
                   static int n = 0;
                   auto msg = std::make_unique<std_msgs::msg::Int32>();
                   msg->data = n++;
                   std::cout << "message publish " << msg->data << std::endl;
                   talker_pub2->publish (std::move(msg));
                 };
  auto talker_timer2 = talker_node2->create_wall_timer(1s, publish2);

  auto callback = [](std_msgs::msg::Int32::UniquePtr msg){
                    std::cout << "message subscribed " << msg->data << std::endl;
                  };
  auto listener_node1 = std::make_shared<rclcpp::Node>(
    "listener_node",  rclcpp::NodeOptions().use_intra_process_comms(true));

  auto listener_sub1 = listener_node1->create_subscription<std_msgs::msg::Int32>
    (topic_name, 1, callback);

  auto listener_node2 = std::make_shared<rclcpp::Node>(
    "listener_node", rclcpp::NodeOptions().use_intra_process_comms(false));

  auto listener_sub2 = listener_node2->create_subscription<std_msgs::msg::Int32>
    (topic_name, 1, callback);

  auto exec = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();
  exec->add_node(talker_node1);
  exec->add_node(talker_node2);
  exec->add_node(listener_node1);
  exec->add_node(listener_node2);

  exec->spin();

  rclcpp::shutdown();
  return 0;
}
