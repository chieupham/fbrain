/*==========================================================================

  © Université de Strasbourg - Centre National de la Recherche Scientifique

  Date: 30/05/2012
  Author(s): Schweitzer Marc (marc.schweitzer@unistra.fr)

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

#ifndef __BTK_MOTIONCORRECTION3DEULERFILTER_H__
#define __BTK_MOTIONCORRECTION3DEULERFILTER_H__

/* ITK */
#include "itkImage.h"
#include "itkImageMaskSpatialObject.h"
#include "itkIdentityTransform.h"
#include "itkTransformFactory.h"
#include "itkAffineTransform.h"
#include "itkEuler3DTransform.h"

/* BTK */
//#include "btkAffineRegistration.h"
#include "btkMacro.h"
#include "btkMotionCorrectionFilter.h"
#include "btkSuperResolutionType.h"


/* OTHERS */
#include "iostream"


namespace btk
{
/**
 * @class MotionCorrection3DEulerFilter
 * @brief The MotionCorrection3DEulerFilter class
 * @author Marc Schweitzer
 * @ingroup SuperResolution
 * @warning This class may be useless...
 */
class MotionCorrection3DEulerFilter: public MotionCorrectionFilter
{

public:

    typedef btk::RigidRegistration< itkImage >     Rigid3DRegistration;


    typedef MotionCorrection3DEulerFilter  Self;
    typedef MotionCorrectionFilter         SuperClass;


    MotionCorrection3DEulerFilter();
    ~MotionCorrection3DEulerFilter();

    virtual void Update();

    virtual void DoRegistration();

   // btkGetMacro(TransformsLR,std::vector< itkTransformBase::Pointer >);
    //btkSetMacro(TransformsLR, std::vector< itkTransformBase::Pointer >);

    virtual std::vector<itkEulerTransform::Pointer> GetOutputTransformsLR()
    {
        return m_OutputTransformsLR;
    }






protected:

    virtual void Initialize();


private:

    std::vector< Rigid3DRegistration::Pointer > m_Rigid3DRegistration;
    //std::vector< itkEulerTransform::Pointer > m_TransformsLR;
    std::vector< itkEulerTransform::Pointer > m_OutputTransformsLR;


    bool    m_UseAffine;
    bool    m_UseEuler;

};
}


#endif
