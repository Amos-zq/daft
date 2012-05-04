/*
* Copyright (C) 2011 David Gossow
*/

#ifndef __DAFT2_STUFF_H__
#define __DAFT2_STUFF_H__

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace cv
{
namespace daft2
{

const float nan = std::numeric_limits<float>::quiet_NaN();

/*
inline void imshow( std::string win_title, cv::Mat img )
{
  cv::Mat img2 = img;
  if ( ( img.type() == CV_32F ) || ( img.type() == CV_64F ) )
  {
    img.convertTo( img2, CV_8U, 255, 0.0);
  }

  //cv::imshow( "x"+ win_title, img2 );
  std::string im_f = "/tmp/img/"+win_title+".png";
  std::cout << "Writing " << im_f << std::endl;
  cv::imwrite( im_f, img2 );
}
*/

inline void imshow2( std::string win_title, cv::Mat img, int size = 256 )
{
  cv::Mat img2;
  if ( size > 0 )
  {
    cv::resize( img, img2, Size( size, size), 0, 0, INTER_NEAREST );
  }
  else
  {
    img2 = img;
  }
  imshow( win_title, img2 );
}

inline void imshowNorm( std::string win_title, cv::Mat1f img, int size = 256 )
{
  double minv,maxv;
  int tmp;
  cv::minMaxIdx( img, &minv, &maxv, &tmp, &tmp );
  imshow2( win_title, (img - minv) * (1.0 / (maxv-minv)), size );
}


Matx33f inline pointCovariance(const std::vector<Vec3f>& points)
{
    float xx=0.0f, xy=0.0f, xz=0.0f, yy=0.0f, yz=0.0f, zz=0.0f;
    for( std::vector<Vec3f>::const_iterator it=points.begin(); it!=points.end(); ++it)
    {
        const Vec3f& p = *it;
        float x = p[0];
        float y = p[1];
        float z = p[2];
        xx += x*x;
        xy += x*y;
        xz += x*z;
        yy += y*y;
        yz += y*z;
        zz += z*z;
    };
    Matx33f A;
    A << xx, xy, xz, xy, yy, yz, xz, yz, zz;
    return A;
}

/** Fits a plane into points and returns the plane normal */
Vec3f inline fitNormal(const std::vector<Vec3f>& points)
{
    Matx33f A = pointCovariance(points);
    Mat1f eigen_vals;
    Mat1f eigen_vecs;
    cv::eigen(A, eigen_vals,eigen_vecs);
    Vec3f normal( eigen_vecs[2][0], eigen_vecs[2][1], eigen_vecs[2][2] );
    if ( normal[2] > 0.0 ) normal *= -1.0;
    return normal;
}


inline float interpBilinear( Mat1f& img, float x, float y )
{
  const int x_low = x;
  const int y_low = y;
  const float tx = x - float(x_low);
  const float ty = y - float(y_low);
  const float v1 = (1.0-tx) * img[y_low][x_low] + tx * img[y_low][x_low+1];
  const float v2 = (1.0-tx) * img[y_low+1][x_low] + tx * img[y_low+1][x_low+1];
  const float v = (1.0-ty) * v1 + ty * v2;
  return v;
}



/** Interpolates linerarly between v1 and v2 given a percentage t */
template<typename T>
inline T interpolateLinear(T t, T v1, T v2)
{
  return (static_cast<T>(1) - t) * v1 + t * v2;
}

template<typename T1,typename T2>
void integral2( const cv::Mat_<T1>& m_in, cv::Mat_<T2>& m_out, T2 factor=1.0 )
{
  m_out.create(m_in.rows + 1, m_in.cols + 1);
  for (int y = 0; y < m_out.rows; y++)
  {
    double row_sum = 0;
    for (int x = 0; x < m_out.cols; x++)
    {
      if (x == 0 || y == 0) {
        m_out[y][x] = 0;
      } else {
        T2 val = factor * static_cast<T2>(m_in[y - 1][x - 1]);
        m_out[y][x] = row_sum + m_out[y - 1][x];
        row_sum += val;
      }
    }
  }
}

inline void depthIntegral( const cv::Mat1f& depth_map, cv::Mat1d& ii_depth_map, cv::Mat_<uint32_t>& ii_depth_count )
{
  ii_depth_map.create(depth_map.rows + 1, depth_map.cols + 1);
  ii_depth_count.create(depth_map.rows + 1, depth_map.cols + 1);

  for (int y = 0; y < depth_map.rows + 1; y++) {
    double row_sum = 0;
    double row_count = 0;
    for (int x = 0; x < depth_map.cols + 1; x++) {
      if (x == 0 || y == 0) {
        ii_depth_map[y][x] = 0;
        ii_depth_count[y][x] = 0;
      } else {
        float depth = depth_map[y - 1][x - 1];
        if (!isnan(depth)) {
          row_sum += depth;
          row_count += 1;
        }
        ii_depth_map[y][x] = row_sum + ii_depth_map[y - 1][x];
        ii_depth_count[y][x] = row_count + ii_depth_count[y - 1][x];
      }
    }
  }
}

// Compute the integral of the rectangle (start_x,start_y),(end_x,end_y)
// using the given integral image
template<typename T>
inline T integrate( const Mat_<T> &ii, int start_x, int start_y, int end_x, int end_y )
{
  assert( start_x>=0 );
  assert( end_x>start_x );
  assert( end_x<ii.cols );
  assert( start_y>=0 );
  assert( end_y>start_y );
  assert( end_y<ii.rows );
  return ii(start_y,start_x) + ii(end_y,end_x) - ii(end_y,start_x) - ii(start_y,end_x);
}

/** Gets integration value of NxN cells of size step x step in a grid starting with point (start_x,start_y)*/
template<typename T, int N>
inline void integrateGridCentered( const Mat_<T> &ii, int start_x, int start_y, int step, float* values) {
//  for(int i=0; i<N; i++) {
//    for(int j=0; j<N; j++) {
//      int x = start_x + step*(j-M);
//      int y = start_y + step*(i-M);
//      values[i*N + j] = integrate(ii, x, x + step, y, y + step);
//    }
//  }
  // look up values in integral image
  float lookup[N+1][N+1];
  for(int i=0; i<N+1; i++) {
    for(int j=0; j<N+1; j++) {
      int x = start_x + step*j - (step*N)/2;
      int y = start_y + step*i - (step*N)/2;
      lookup[i][j] = ii(y, x);
    }
  }
  // compute cell integrals
  for(int i=0; i<N; i++) {
    for(int j=0; j<N; j++) {
      values[i*N + j] = lookup[i][j] + lookup[i+1][j+1] - lookup[i+1][j] - lookup[i][j+1];
    }
  }
}

// compute the area of the given rect
inline double area(int start_x, int end_x, int start_y, int end_y)
{
  return (end_y-start_y) * (end_y-start_y);
}

// return false if the square at (x,y) with size s*2 intersect the image border
template<typename T>
inline bool checkBounds ( const Mat_<T> &ii, int x, int y, int s )
{
  return ( (s > 0) && (x > s) && (x + s < ii.cols) && (y > s) && (y + s < ii.rows) );
}

inline float getScale( float fac, float base_scale )
{
  return fac*base_scale;
}

// compute 3d point from pixel position, depth and camera intrinsics
// @param f_inv: 1/f
// @param cx,cy optical center
// @param u,v pixel coords
// @param p output point in 3d
inline void getPt3d( float f_inv, float cx, float cy, float u, float v, float z, Point3f& p )
{
  float zf = z*f_inv;
  p.x = zf * (u-cx);
  p.y = zf * (v-cy);
  p.z = z;
}

inline void getPt2d( const Point3f& p, float f, float cx, float cy, Point2f& v )
{
  v.x = p.x * f / p.z + cx;
  v.y = p.y * f / p.z + cy;
}

// approximate 1 / sqrt(x) with accuracy ~2%
inline float fastInverseSqrt(float x)
{
  uint32_t i = *((uint32_t *)&x);            // evil floating point bit level hacking
  i = 0x5f3759df - (i >> 1);                // use a magic number
  float s = *((float *)&i);                // get back guess
  return s * (1.5f - 0.5f * x * s * s);    // one newton iteration
}

// compute 1/length(p)
inline float fastInverseLen( const Point2f& p )
{
  return fastInverseSqrt( p.x*p.x + p.y*p.y );
}

// compute 1/length(p)
inline float fastInverseLen( const Point3f& p )
{
  return fastInverseSqrt( p.x*p.x + p.y*p.y + p.z*p.z );
}


inline float meanDepth(const Mat1d &ii_depth_map,
    const cv::Mat_<uint32_t>& ii_depth_count,
    int x, int y, int sp_int )
{
  int x_left = x-sp_int;
  int x_right = x+sp_int;
  int y_top = y-sp_int;
  int y_bottom = y+sp_int;
  if ( x_left < 0 ) x_left = 0;
  if ( y_top < 0 ) y_top = 0;
  if ( x_right >= ii_depth_map.cols ) x_right = ii_depth_map.cols-1;
  if ( y_bottom >= ii_depth_map.rows ) y_bottom = ii_depth_map.rows-1;
  float nump = float(integrate( ii_depth_count, x_left, y_top, x_right, y_bottom ));
  if ( nump == 0 )
  {
    return std::numeric_limits<float>::quiet_NaN();
  }
  return integrate( ii_depth_map, x_left, y_top, x_right, y_bottom ) / nump;
}

/** compute depth gradient
 * @param sp step width in projected pixel
 */
inline bool computeGradient(
    const Mat1f &depth_map, int x, int y, float sp, Vec2f& grad)
{
  int sp_int = int(sp+0.5f);

  if ( !checkBounds( depth_map, x, y, sp_int ) )
  {
    grad[0] = grad[1] = nan;
    return false;
  }

  // get depth values from image
  const float d_center = depth_map(y,x);
  const float d_xp = depth_map(y,x+sp_int);
  const float d_yp = depth_map(y+sp_int,x);
  const float d_xn = depth_map(y,x-sp_int);
  const float d_yn = depth_map(y-sp_int,x);

  if ( isnan(d_center) || isnan(d_xp) || isnan(d_yp) || isnan(d_xn) || isnan(d_yn) )
  {
    grad[0] = grad[1] = nan;
    return false;
  }

  const float fac = 0.5*sp/float(sp_int);
  grad[0] = (d_xp - d_xn)*fac;
  grad[1] = (d_yp - d_yn)*fac;
  return true;
}

/** Computes A*x^2 + B*x*y + C*y^2 form of ellipse from angle and major/minor axis length */
inline void computeEllipseParams(float angle, float major, float minor, float& A, float& B, float& C)
{
  float ax = std::cos(angle);
  float ay = std::sin(angle);
  float bx = -ay;
  float by = ax;

  float a2 = major * major;
  float b2 = minor * minor;

  A = ax*ax / a2 + bx*bx / b2;

  B = 2.0f * (ax*ay / a2 + bx*by / b2);

  C = ay*ay / a2 + by*by / b2;
}

/** Checks if a point (x,y) is contained in an ellipse of form A*x^2 + B*x*y + C*y^2 */
inline bool ellipseContains(float x, float y, float A, float B, float C)
{
  return A*x*x + B*x*y + C*y*y <= 1.0f;
}

}
}

#endif
