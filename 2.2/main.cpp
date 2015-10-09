#include <vector>
#include <cstdio>
#include <algorithm>
#include <memory>
#include <IL/il.h>
#include <glog/logging.h>
using std::vector;
using std::equal;
using std::copy;
using std::unique_ptr;

struct Rotate {
	// w: image width
	// h: image height
	// bpp: byte per pixel
	void Run(unsigned char *image, int n, int bpp)
	{
		// You are not allowed to allocate other memory
		unique_ptr<unsigned char[]> tmp(new unsigned char[bpp]);
    for(int i = 0; i < n/2; i++){
      for(int j = i; j < n-1-i; j++){
        for(int k = 0; k < bpp; k++){
          //lu to tmp
          tmp[k] = image[i*n*bpp + j*bpp + k];
          //ld to lu
          image[i*n*bpp + j*bpp + k] = image[(n-j-1)*n*bpp + i*bpp + k];
          //rd to ld
          image[(n-j-1)*n*bpp + i*bpp + k] = image[(n-i)*n*bpp - (j+1)*bpp + k];
          //ru to rd
          image[(n-i)*n*bpp - (j+1)*bpp + k] = image[(j+1)*n*bpp - (i+1)*bpp + k];
          //tmp to ru
          image[(j+1)*n*bpp - (i+1)*bpp + k] = tmp[k];
        }
      }
    }
  }
};

#define IL_CHECK_ERROR() {auto e = ilGetError();CHECK_EQ(e, IL_NO_ERROR)<<e;}
int main(int argc, char const* argv[])
{
	google::InitGoogleLogging(argv[0]);
	FLAGS_logtostderr = true;
	ilInit();

	LOG(INFO) << "Using devil library version " << ilGetInteger(IL_VERSION_NUM);

	// Allocate images
	ILuint image;
	ilGenImages(1, &image);
	ilBindImage(image);
	IL_CHECK_ERROR();

	// Read image
	ilLoadImage("lena_color.bmp");
	IL_CHECK_ERROR();
	auto bpp = ilGetInteger(IL_IMAGE_BPP); // bpp = byte per pixels
  auto w = ilGetInteger(IL_IMAGE_WIDTH);
	auto h = ilGetInteger(IL_IMAGE_HEIGHT);
	LOG(INFO) << "Load image width = " << w << ", height = " << h;
	CHECK_EQ(w, h) << "We are testing w == h now.";
	ILubyte *color_img_ptr = ilGetData();
	vector<ILubyte> backup(color_img_ptr, color_img_ptr+w*w*bpp);
	IL_CHECK_ERROR();

	// Rotate!
	Rotate r;
	r.Run(color_img_ptr, w, bpp);

	for (int i = 0; i < w; ++i) {
		const unsigned char *ptr_original = backup.data() + w*i*bpp;
		const unsigned char *ptr_rotated = color_img_ptr + (w-i-1)*bpp;
		for (int j = 0; j < w; ++j) {
			if (!equal(ptr_original, ptr_original+bpp, ptr_rotated)) {
				LOG_FIRST_N(WARNING, 100) << "Mismatch at " << i << ", " << j;
			}
			ptr_original += bpp;
			ptr_rotated += w*bpp;
		}
	}

	// store image
	ilEnable(IL_FILE_OVERWRITE);
	ilSaveImage("lena_r90.bmp");
	IL_CHECK_ERROR();

	ilDeleteImages(1, &image);

	// 1 more small testing
	unsigned char a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	unsigned char b[] = {7, 4, 1, 8, 5, 2, 9, 6, 3};
	r.Run(a, 3, 1);
	LOG_IF(WARNING, !equal(a, a+9, b)) << "You do not pass the small testcase";

	return 0;
}
