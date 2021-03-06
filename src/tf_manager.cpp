/****************************************************************************************************
 *  Software License Agreement (BSD License)
 *
 *  Copyright 2017, Andy McEvoy
 *
 *  Redistribution and use in source and binary forms, with or without modification, are permitted
 *  provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this list of conditions
 *  and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice, this list of
 *  conditions and the following disclaimer in the documentation and/or other materials provided
 *  with the distribution.
 *
 *  3. Neither the name of the copyright holder nor the names of its contributors may be used to
 *  endorse or promote products derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 *  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 *  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 *  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 *  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************************************/

/*
 * Author(s) : Andy McEvoy ( mcevoy.andy@gmail.com )
 * Desc      : implementation of rviz_tf_publisher.h. See *.h file for documentation.
 * Created   : 2017 - May - 18
 */

#include <tf_visual_tools/tf_manager.h>

namespace tf_visual_tools
{

RvizTFPublisher::RvizTFPublisher()
  : nh_("~")
{
  //each of the create, update etc methods publish to a unique topic when the relevant button is
  //pressed in the GUI. Here, we subscribe to these topics to do the essential callbacks
  create_tf_sub_ = nh_.subscribe("/rviz_tf_create", 100, &RvizTFPublisher::createTF, this);
  remove_tf_sub_ = nh_.subscribe("/rviz_tf_remove", 100, &RvizTFPublisher::removeTF, this);
  update_tf_sub_ = nh_.subscribe("/rviz_tf_update", 100, &RvizTFPublisher::updateTF, this);
  include_tf_sub_ = nh_.subscribe("/rviz_tf_include", 100, &RvizTFPublisher::includeTF, this);
}

void RvizTFPublisher::createTF(geometry_msgs::TransformStamped create_tf_msg)
{
  active_tfs_.push_back(create_tf_msg);
}

void RvizTFPublisher::removeTF(geometry_msgs::TransformStamped remove_tf_msg)
{
  for (std::size_t i = 0; i < active_tfs_.size(); i++)
  {
    if (remove_tf_msg.child_frame_id.compare(active_tfs_[i].child_frame_id) == 0 &&
        remove_tf_msg.header.frame_id.compare(active_tfs_[i].header.frame_id) == 0)
    {
      active_tfs_.erase(active_tfs_.begin() + i);
      break;
    }
  }
}

void RvizTFPublisher::updateTF(geometry_msgs::TransformStamped update_tf_msg)
{
  for (std::size_t i = 0; i < active_tfs_.size(); i++)
  {
    if (update_tf_msg.child_frame_id.compare(active_tfs_[i].child_frame_id) == 0 &&
        update_tf_msg.header.frame_id.compare(active_tfs_[i].header.frame_id) == 0)
    {
      active_tfs_[i].transform = update_tf_msg.transform;
    }
  }
}

void RvizTFPublisher::includeTF(geometry_msgs::TransformStamped include_tf_msg)
{

  active_tfs_.push_back(include_tf_msg);
}

//Publish transforms to /tf_static
void RvizTFPublisher::publishTFs()
{
  //static tf::TransformBroadcaster br;
  static tf2_ros::StaticTransformBroadcaster static_br;
  for (std::size_t i = 0; i < active_tfs_.size(); i++)
  {
    //tf::StampedTransform tf;
    geometry_msgs::TransformStamped static_transformStamped;

    active_tfs_[i].header.stamp = ros::Time::now();
    //tf::transformStampedMsgToTF(active_tfs_[i], tf);
    //In case the above tf function is not compatible with static transforms
    //tf2::fromMsg(active_tfs_[i], static_transformStamped);

    static_transformStamped.header.stamp = active_tfs_[i].header.stamp;
    static_transformStamped.header.frame_id = active_tfs_[i].header.frame_id;
    static_transformStamped.child_frame_id = active_tfs_[i].child_frame_id;
    static_transformStamped.transform.translation.x = active_tfs_[i].transform.translation.x;
    static_transformStamped.transform.translation.y = active_tfs_[i].transform.translation.y;
    static_transformStamped.transform.translation.z = active_tfs_[i].transform.translation.z;
    static_transformStamped.transform.rotation.x = active_tfs_[i].transform.rotation.x;
    static_transformStamped.transform.rotation.y = active_tfs_[i].transform.rotation.y;
    static_transformStamped.transform.rotation.z = active_tfs_[i].transform.rotation.z;
    static_transformStamped.transform.rotation.w = active_tfs_[i].transform.rotation.w;

    static_br.sendTransform(static_transformStamped);
  }
}
} // end namespace tf_visual_tools
