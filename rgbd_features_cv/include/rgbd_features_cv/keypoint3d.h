/*
 * RGBD Features -> OpenCV bridge
 * Copyright (C) 2011 David Gossow
*/

#ifndef __cv_keypoint3d_h__
#define __cv_keypoint3d_h__

#include <opencv2/features2d/features2d.hpp>

namespace cv
{

/*!
  RGBD features implementation.
*/
struct KeyPoint3D: public cv::KeyPoint
{
  KeyPoint3D(float x, float y, float _size, float _world_size,
      float _angle=-1,
      float _response=0, int _octave=0, int _class_id=-1)
      : KeyPoint( x ,y, _size, _angle, _response, _octave, _class_id),
        world_size(_world_size), affine_angle(0), affine_major(0), affine_minor(0) {}

  float world_size; //!< diameter (in meters) of the meaningful keypoint neighborhood

  CV_PROP_RW Point3f pt3d; //!< 3D position in the camera frame
  CV_PROP_RW Point3f normal; //!< 3D normal in camera coords

  // affine transformation
  CV_PROP_RW float affine_angle;
  CV_PROP_RW float affine_major;
  CV_PROP_RW float affine_minor;
};

void drawKeypoints3D( const Mat& image, const vector<KeyPoint3D>& keypoints, Mat& outImage,
    const Scalar& color=Scalar::all(-1), int flags=DrawMatchesFlags::DEFAULT );

// convert 3d-keypoints into regular ones
vector<KeyPoint> makeKeyPoints( vector<KeyPoint3D> kp );

}

#endif
