/*
Copyright or © or Copr. Université de Strasbourg - Centre National de la Recherche Scientifique

24 january 2013
rousseau@unistra.fr

This software is governed by the CeCILL-B license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL-B
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL-B license and that you accept its terms.
*/

#ifndef BTK_PATCH2_H
#define BTK_PATCH2_H

#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

#include "btkMacro.h"

namespace btk
{

  /**
  * @class Patch2
  * @brief 3D Patch
  * @author François Rousseau
  * @ingroup Tools
  */
  template<typename T>
  class Patch2
  {
    public:
    //Defining ITK stuff for the input image and the patch image
      typedef typename itk::Image< T, 3>                          itkTImage;
      typedef typename itkTImage::Pointer                         itkTImagePointer;
      typedef typename itk::ImageRegionIterator< itkTImage >      itkTIterator;
      typedef typename itk::ImageRegionConstIterator< itkTImage > itkConstIterator;
	  typedef typename itkTImage::SpacingType                     itkTSpacing;    
    

      //Get and Set functions


      Patch2(){};
      Patch2(itkTImagePointer & image, typename itkTImage::SizeType & fullPatchSize){ CreateEmptyPatch(image,fullPatchSize); };
      Patch2(itkTImagePointer & image, typename itkTImage::SizeType & fullPatchSize, typename itkTImage::IndexType & p){ CreateEmptyPatch(image,fullPatchSize); ComputePatch(image, p);};

      void CreateEmptyPatch(itkTImagePointer & image, typename itkTImage::SizeType & fullPatchSize);
      void ComputePatch(itkTImagePointer & image, typename itkTImage::IndexType & p);
      void ComputePatchRegion(itkTImagePointer & image, typename itkTImage::IndexType & p, typename itkTImage::RegionType & imageRegion, typename itkTImage::RegionType & patchRegion);

      void ComputeMeanAndVariance(float &mean, float &variance);
      T GetDataAtCenter();

      btkSetMacro(CentralPointInImage,typename itkTImage::RegionType::IndexType);
      btkGetMacro(CentralPointInImage,typename itkTImage::RegionType::IndexType);
      btkSetMacro(Data, itkTImagePointer);
      btkGetMacro(Data, itkTImagePointer);

	private:

      itkTImagePointer                          m_Data;                //Data, i.e. ITK object containing image intensities
      typename itkTImage::RegionType::IndexType m_CentralPointInImage; //Coordinates of the central point of a patch in the image
  
  };

} // namespace btk

#ifndef ITK_MANUAL_INSTANTIATION
#include "btkPatch2.txx"
#endif

#endif // BTK_PATCH2_H
