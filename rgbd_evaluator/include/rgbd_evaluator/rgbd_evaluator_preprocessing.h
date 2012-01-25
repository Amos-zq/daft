/*
 * rgbd_evaluator_preprocessing.h
 *
 *  Created on: Jan 13, 2012
 *      Author: praktikum
 */
#ifndef RGBD_EVALUATOR_PREPROCESSING_H_
#define RGBD_EVALUATOR_PREPROCESSING_H_

#include <ros/ros.h>
#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <rosbag/message_instance.h>

#include <boost/foreach.hpp>

#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>

#include <sensor_msgs/Image.h>
#include <geometry_msgs/TransformStamped.h>
#include <sensor_msgs/CameraInfo.h>

#include <LinearMath/btTransform.h>

#include <cv_bridge/cv_bridge.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <tf/tf.h>


namespace rgbd_evaluator
{

class RgbdEvaluatorPreprocessing
{
public:

  RgbdEvaluatorPreprocessing(std::string);
  virtual ~RgbdEvaluatorPreprocessing();

  void createTestFiles();
  void calculateHomography();

private:

  cv::Matx33f calculateInitialHomography(btTransform transform_camx_to_original, btTransform transform_original);
  void printMat(cv::Matx33f M);
  void writeHomographyToFile(cv::Matx33f homography, uint32_t count);
  void createFileName(char* s);

  std::string bagfile_name_;
  rosbag::Bag bag_;
  cv_bridge::CvImagePtr tmp_image_;
  cv::Matx33f K;

  static const uint32_t BUFF_SIZE = 500;

  struct ImageData {
    boost::shared_ptr<cv_bridge::CvImage> image;
    boost::shared_ptr<btTransform> approx_transform;
    //...

    bool isComplete() {
      return image.get() && approx_transform.get();
    }
  };

  std::vector< ImageData > image_store_;
};

}
#endif /* RGBD_EVALUATOR_PREPROCESSING_H_ */
