#ifndef RANDOMCROPPER_CXX
#define RANDOMCROPPER_CXX

#include <random>
#include "RandomCropper.h"

namespace larcv {

  RandomCropper::RandomCropper(const std::string name)
    : larcv_base(name)
    , _target_rows(0)
    , _target_cols(0)
    , _crop_rows(0)
    , _crop_cols(0)
    , _randomize_crop(false)
  {
    _crop_cols = _crop_rows = 0;
    _randomize_crop = false;
  }
  
  void RandomCropper::configure(const PSet& cfg)
  {
    _randomize_crop = cfg.get<bool>("RandomizeCrop",false);
    
    _crop_cols      = cfg.get<int>("CroppedCols");
    _crop_rows      = cfg.get<int>("CroppedRows");

    
    _downsize_cols = cfg.get<int>("DownsizedCols");
    _downsize_rows = cfg.get<int>("DownsizedRows");

  }

  void RandomCropper::set_crop_region(const size_t rows, const size_t cols)
  {


    _target_rows = rows;
    _target_cols = cols;
    _col_offset = _row_offset = 0;
    int coldiff = std::max(0,(int)_target_cols - (int)_crop_cols);
    int rowdiff = std::max(0,(int)_target_rows - (int)_crop_rows);

    if ( _randomize_crop ) {

      //      std::cout << "RANDOMIZE_CROP" << std::endl; //yj

      std::random_device rd;
      std::mt19937 gen(rd());
      if ( coldiff>0 ) {
        std::uniform_int_distribution<> irand_col(0,coldiff);
        _col_offset = irand_col(gen);
      }
      
      if ( rowdiff>0 ) {
        std::uniform_int_distribution<> irand_row(0,rowdiff);
        _row_offset = irand_row(gen);
      }
    }
    else {
      if ( coldiff>0 ) _col_offset = (int)coldiff/2;
      if ( rowdiff>0 ) _row_offset = (int)rowdiff/2;
    }

  }


  const std::vector<float> RandomCropper::cut_empty(const larcv::Image2D& image){
  
    Image2D temp_img = image;

    temp_img.resize((size_t)480,(size_t)560,(float)0);

    _cut_empty_image = temp_img.as_vector();
    
    //temp_img.~Image2D();
    return _cut_empty_image;
  }


  const std::vector<float> RandomCropper::crop(const larcv::Image2D& image)
  {
    // Check input image size
    if(image.meta().rows() != _target_rows ||
       image.meta().cols() != _target_cols) { 

      LARCV_CRITICAL() << "Image dimension (" << image.meta().rows() 
		       << "," << image.meta().cols() << ") does not match with what is specified ("
		       << _target_rows << "," << _target_cols << ")!" << std::endl;

      throw larbys();
    }

    // Set _cropped_image size
    size_t cropped_rows = std::min(_target_rows, _crop_rows);
    size_t cropped_cols = std::min(_target_cols, _crop_cols);

    _cropped_image.resize( cropped_rows * cropped_cols );

    auto const& data_v = image.as_vector();

    for(size_t col = 0; col < cropped_cols; ++col) {
      for(size_t row = 0; row < cropped_rows; ++row) {
        _cropped_image[cropped_rows * col + row] = data_v[(col + _col_offset) * _target_rows + row + _row_offset];
      }
    }
    
    return _cropped_image;
  }


  const std::vector<float> RandomCropper::average_pull(const larcv::Image2D& image)
  {
      Image2D temp_img = image;

      bool CutEmptyPixelsSquare = true;
     
      if(CutEmptyPixelsSquare) {

	//	std::cout << "Hm : " << _crop_rows << " , " << _crop_cols << std::endl;
	temp_img.resize(_crop_rows, _crop_cols, 0.);
      }

      auto const& data_v = temp_img.as_vector();
      //std::cout << "Hm : " << temp_img.meta().rows() << " , " << temp_img.meta().cols() << std::endl;

      
      if(_crop_rows/_downsize_rows  == 2 && _crop_cols/_downsize_cols == 2 ){

	_average_pull_image.resize(int(temp_img.meta().rows()/2)*int(temp_img.meta().cols()/2));
	int index_k = 0;
	for (int i = 0 ; i < temp_img.meta().rows(); i+=2){       
	  for (int j =0 ; j < temp_img.meta().cols(); j+=2){
	    //	  std::cout << " (i,j) = " << i<< ","<<j << std::endl;
	    int idx11 = i*temp_img.meta().cols()+j;
	    int idx12 = i*temp_img.meta().cols()+(j+1);
	    int idx21 = (i+1)*temp_img.meta().cols() + j;
	    int idx22 = (i+1)*temp_img.meta().cols() + (j+1);
	    float temp_ave = 0.25*(data_v[idx11]+data_v[idx12] + data_v[idx21] + data_v[idx22] ) ;

	    _average_pull_image[index_k] = temp_ave;
	  
	    index_k ++;
	  }
	}

      }
      else if(_crop_rows/_downsize_rows == 4 && _crop_cols/_downsize_cols == 4) {

	_average_pull_image.resize(int(temp_img.meta().rows()/4)*int(temp_img.meta().cols()/4));
	int index_k = 0;
	for (int i = 0 ; i < temp_img.meta().rows(); i+=4){       
	  for (int j =0 ; j < temp_img.meta().cols(); j+=4){
	    //	  std::cout << " (i,j) = " << i<< ","<<j << std::endl;
	    int idx11 = i*temp_img.meta().cols()+j;
	    int idx12 = i*temp_img.meta().cols()+(j+1);
	    int idx13 = i*temp_img.meta().cols()+(j+2);
	    int idx14 = i*temp_img.meta().cols()+(j+3);
	  
	    int idx21 = (i+1)*temp_img.meta().cols() + j;
	    int idx22 = (i+1)*temp_img.meta().cols() + (j+1);
	    int idx23 = (i+1)*temp_img.meta().cols() + (j+2);
	    int idx24 = (i+1)*temp_img.meta().cols() + (j+3);

	    int idx31 = (i+2)*temp_img.meta().cols()+j;
	    int idx32 = (i+2)*temp_img.meta().cols()+(j+1);
	    int idx33 = (i+2)*temp_img.meta().cols()+(j+2);
	    int idx34 = (i+2)*temp_img.meta().cols()+(j+3);
	  
	    int idx41 = (i+3)*temp_img.meta().cols() + j;
	    int idx42 = (i+3)*temp_img.meta().cols() + (j+1);
	    int idx43 = (i+3)*temp_img.meta().cols() + (j+2);
	    int idx44 = (i+3)*temp_img.meta().cols() + (j+3);



	    float temp_ave = ( data_v[idx11]+data_v[idx12]+data_v[idx13]+data_v[idx14]+ data_v[idx21]+data_v[idx22]+data_v[idx23]+data_v[idx24]+data_v[idx31]+data_v[idx32]+data_v[idx33]+data_v[idx34]+data_v[idx41]+data_v[idx42]+data_v[idx43]+data_v[idx44] )/16. ;

	    _average_pull_image[index_k] = temp_ave;
	  
	    index_k ++;
	  }
	}


      }

      return _average_pull_image;
  }



  const std::vector<float> RandomCropper::downsize(const larcv::Image2D& image)
  {

    Image2D temp_img = image;

    //    std::cout << " tmep_img = imga" << std::endl;

    bool CutEmptyPixels =false; // move the definition later
    
    bool CutEmptyPixelsSquare = true;
    if (CutEmptyPixels) {
      //std::cout << " can we cut empty pixels ? " << std::endl;
      temp_img.resize((size_t)480,(size_t)560,(float)0);
      //std::cout << " We are cutting empty pixels." << std::endl;
    }

    else if(CutEmptyPixelsSquare) {

      //std::cout << "Hm : " << _crop_rows << " , " << _crop_cols << std::endl;
      temp_img.resize(_crop_rows, _crop_cols, 0.);
    }

      

    size_t downsized_rows = _downsize_rows;
    size_t downsized_cols = _downsize_cols;

    _downsized_image.resize( downsized_rows * downsized_cols );


    //int getIndex(int w, int h, int stride){ return h*stride+w ;}
    //    char * temp = new char[downsized_rows*downsized_cols];

    float scale_row = temp_img.meta().rows()/(float)downsized_rows;
    float scale_col = temp_img.meta().cols()/(float)downsized_cols;

    int tSrc_row =0, tSrc_col = 0;
    int index_src =0, index_dest =0;

    auto const& data_v = temp_img.as_vector();

    for (int i = 0; i< downsized_rows; i++){
      //Find the nearest y-pos in the original image
      tSrc_row = (int)(scale_row * i + 0.5); 
      for(int j =0; j < downsized_cols; j++){
	tSrc_col = (int)(scale_col * j +0.5);

	//getIndex(tSrc_col, tSrc_row, )
	index_src = tSrc_row*temp_img.meta().cols() +tSrc_col;
	index_dest = downsized_cols*i  + j ;

	_downsized_image[index_dest] = data_v[index_src];
	
      }
    }
    
    //temp_img.~Image2D();


    return _downsized_image;
    

 
    //trying something from internet
    //int[] temp = new int[_downsize_rows * _downsize_cols];
    // EDIT: added +1 to account for an early rounding problem

    /*
    int rows_ratio = (int) ( (temp_img.meta().rows()<<16)/_downsize_rows ) +1 ;
    int cols_ratio = (int) ( (temp_img.meta().cols()<<16)/_downsize_cols ) +1 ;

    std::cout << "downsized_rows : " << downsized_rows << " ,  rows_ratio : " << rows_ratio << std::endl;
  

    auto const& data_v = temp_img.as_vector();
    std::cout << " size of data_v : " << std::endl;
  
    int row2, col2;
    
    for(int i=0; i<downsized_rows; i++){
      for(int j=0; j<downsized_cols; j++){
	row2 = ( (j*rows_ratio)>>16 );
	col2 = ( (i*cols_ratio)>>16 );
	_downsized_image[(i*downsized_rows)+j] = data_v[col2*temp_img.meta().rows()+row2];
              
      }
          
    }

    return _downsized_image;

    */


  }

}
#endif
