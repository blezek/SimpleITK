/*=========================================================================
*
*  Copyright Insight Software Consortium
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*         http://www.apache.org/licenses/LICENSE-2.0.txt
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*=========================================================================*/
#include "itkImage.h"
#include "itkVectorImage.h"
#include "itkLabelMap.h"
#include "itkLabelObject.h"
#include "itkNumericTraits.h"
#include "itkNumericTraitsVariableLengthVectorPixel.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "itkComposeImageFilter.h"

#include "sitkComposeImageFilter.h"
#include "itkComposeImageFilter.h"



namespace itk {
namespace simple {

//-----------------------------------------------------------------------------

//
// Default constructor that initializes parameters
//
ComposeImageFilter::ComposeImageFilter ()
{

  this->m_MemberFactory.reset( new detail::MemberFunctionFactory<MemberFunctionType>( this ) );

  this->m_MemberFactory->RegisterMemberFunctions< PixelIDTypeList, 2 > ();
  this->m_MemberFactory->RegisterMemberFunctions< PixelIDTypeList, 3 > ();

}

//
// ToString
//
std::string ComposeImageFilter::ToString() const
{
  std::ostringstream out;
  out << "itk::simple::ComposeImageFilter\n";


  return out.str();
}

Image ComposeImageFilter::Execute ( const Image &image )
{
  Image constImage = image;
  std::vector<Image> images;
  images.push_back(constImage);
  return this->Execute( images );
}

//
// Execute
//
Image ComposeImageFilter::Execute ( const std::vector<Image> &images )
{
  if ( images.empty() )
    {
    sitkExceptionMacro( "Atleast one input is required" );
    }

  PixelIDValueType type = images.front().GetPixelIDValue();
  unsigned int dimension = images.front().GetDimension();

  return this->m_MemberFactory->GetMemberFunction( type, dimension )( images );
}

//-----------------------------------------------------------------------------

//
// ExecuteInternal
//
template <class TImageType>
Image ComposeImageFilter::ExecuteInternal ( const std::vector<Image> & images  )
{
  // Define the input and output image types
  typedef TImageType     InputImageType;

  typedef itk::VectorImage<typename InputImageType::PixelType, InputImageType::ImageDimension> OutputImageType;

  typedef itk::ComposeImageFilter<InputImageType,  OutputImageType> FilterType;
  // Set up the ITK filter
  typename FilterType::Pointer filter = FilterType::New();

  for ( unsigned int i = 0; i < images.size(); ++i )
    {
    // Get the pointer to the ITK image contained in image1
    typename InputImageType::ConstPointer image = this->CastImageToITK<InputImageType>( images[i] );

    filter->SetInput( i, image );
    }


  // Run the ITK filter and return the output as a SimpleITK image
  filter->Update();


  return Image( filter->GetOutput() );
}

//-----------------------------------------------------------------------------


//
// Function to run the Execute method of this filter
//
Image Compose ( const std::vector<Image>& images )
{
  ComposeImageFilter filter;
  return filter.Execute ( images );
}

} // end namespace simple
} // end namespace itk
