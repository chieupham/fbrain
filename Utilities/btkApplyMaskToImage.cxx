/*==========================================================================

  © Université de Strasbourg - Centre National de la Recherche Scientifique

  Date: 26/06/2012
  Author(s): Marc Schweitzer (marc.schweitzer@unistra.fr)

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

==========================================================================*/

/* ITK */
#include "itkImage.h"
#include "itkMaskImageFilter.h"
#include "itkImageIOBase.h"
#include "itkExtractImageFilter.h"
#include "itkJoinSeriesImageFilter.h"

/* BTK */
#include "btkImageHelper.h"

/* OTHERS */
#include "iostream"
#include <tclap/CmdLine.h>


int main(int argc, char * argv[])
{
    TCLAP::CmdLine cmd("btkApplyMaskToImage: Apply a 3D mask(non-zero values) to a 3D or 4D image", ' ', "1.0", true);
    TCLAP::ValueArg<std::string> inputImageArg("i","image_file","input image file (short)",true,"","string", cmd);
    TCLAP::ValueArg<std::string> outputImageArg("o","output_file","output image file (short)",true,"","string", cmd);
    TCLAP::ValueArg<std::string> inputMaskArg("m","mask_file","filename of the mask image (dimension = 3)",false,"","string", cmd);

    // Parse the args.
    cmd.parse( argc, argv );    

    // Get the value parsed by each arg.
    std::string input_file       = inputImageArg.getValue();
    std::string output_file      = outputImageArg.getValue();
    std::string mask_file        = inputMaskArg.getValue();


    //Determine the input image dimension
    itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(input_file.c_str(), itk::ImageIOFactory::ReadMode);
    imageIO->SetFileName(input_file);
    imageIO->ReadImageInformation();

    unsigned int Dimension = imageIO->GetNumberOfDimensions();
    std::cout<<"Dimension of input image : "<<Dimension<<"\n";

    typedef short PixelType;
    typedef itk::Image<PixelType, 3> itk3DImage;
    typedef itk::Image<PixelType, 4> itk4DImage;
    typedef itk::Image<unsigned char, 3> itk3DMask;
    typedef itk::MaskImageFilter<itk3DImage,itk3DMask, itk3DImage> MaskImageFilter;
    
    itk3DMask::Pointer mask   = itk3DMask::New();
    mask  = btk::ImageHelper<itk3DMask>::ReadImage(mask_file);

    
    switch(Dimension)
    {
      case 3:
      {
        itk3DImage::Pointer image = itk3DImage::New();

        //read input image and mask
        image = btk::ImageHelper<itk3DImage>::ReadImage(input_file);
    
        MaskImageFilter::Pointer maskImageFilter = MaskImageFilter::New();
        maskImageFilter->SetMaskImage(mask);        
        maskImageFilter->SetInput(image);
        maskImageFilter->Update();

        //write the result
        btk::ImageHelper<itk3DImage>::WriteImage(maskImageFilter->GetOutput(),output_file);
           
      }
        break;
      case 4:
      {
        //For 4D image, we apply the ITK maskImageFilter for each of the 3D images and then concat the output images        
        
        typedef itk4DImage::Pointer itk4DImagePointer;
        typedef itk3DImage::Pointer itk3DImagePointer;

        typedef itk::ImageFileReader< itk4DImage >  Reader4DType;
        typedef itk::ImageFileWriter< itk4DImage >  Writer4DType;
        
        typedef itk::ExtractImageFilter< itk4DImage, itk3DImage > ExtractImageFilterType;
        ExtractImageFilterType::Pointer extractor  = ExtractImageFilterType::New();

        typedef itk::JoinSeriesImageFilter< itk3DImage, itk4DImage > JoinSeriesFilterType;
        JoinSeriesFilterType::Pointer concatenator  = JoinSeriesFilterType::New();
    
        
        Reader4DType::Pointer reader = Reader4DType::New();
        reader->SetFileName(input_file);
        reader->Update();
        
        extractor->SetInput( reader->GetOutput() );
        
        itk4DImage::RegionType input4DRegion = reader->GetOutput()->GetLargestPossibleRegion();
        itk4DImage::SizeType input4DSize = input4DRegion.GetSize();

        itk4DImage::SizeType desired3DSize;
        desired3DSize[0] = input4DSize[0];
        desired3DSize[1] = input4DSize[1];
        desired3DSize[2] = input4DSize[2];
        desired3DSize[3] = 0;

        concatenator->SetOrigin( reader->GetOutput()->GetOrigin()[3] );
        concatenator->SetSpacing( reader->GetOutput()->GetSpacing()[3] );

        itk4DImage::IndexType start = input4DRegion.GetIndex();
        uint numberOf3Dimages = input4DSize[3];
    
        for (uint i = 0; i < numberOf3Dimages; i++){

          start[3] = i;

          itk4DImage::RegionType desiredRegion;
          desiredRegion.SetSize(  desired3DSize  );
          desiredRegion.SetIndex( start );

          extractor->SetExtractionRegion( desiredRegion );
          extractor->SetDirectionCollapseToSubmatrix();
          extractor->Update();
          itk3DImagePointer input3DImage = extractor->GetOutput();

          itk3DImagePointer output3DImage = itk3DImage::New();
          
          MaskImageFilter::Pointer maskImageFilter = MaskImageFilter::New();
          maskImageFilter->SetMaskImage(mask);
          maskImageFilter->SetInput(input3DImage);
          maskImageFilter->Update();
          output3DImage = maskImageFilter->GetOutput();

          concatenator->PushBackInput(output3DImage);
        }


        //Write the result

        Writer4DType::Pointer writer = Writer4DType::New();
        writer->SetFileName( output_file );
        writer->SetInput( concatenator->GetOutput() );
        writer->Update();                    
      }
        break;
      default:
        std::cout<<"Only dimension equal to 3 or 4 is supported.\n";      
    }

    return EXIT_SUCCESS;
}
