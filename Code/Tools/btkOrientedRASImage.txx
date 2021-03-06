/*==========================================================================

  © Université de Strasbourg - Centre National de la Recherche Scientifique

  Date: 09/01/2013
  Author(s):Marc Schweitzer (marc.schweitzer(at)unistra.fr)

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

#ifndef BTK_ORIENTEDRASIMAGE_TXX
#define BTK_ORIENTEDRASIMAGE_TXX

#include "btkOrientedRASImage.h"

namespace btk
{
//-------------------------------------------------------------------------------------------------
template <class TPixel, unsigned int Dimension>
OrientedRASImage<TPixel, Dimension>::OrientedRASImage()
{

}
//-------------------------------------------------------------------------------------------------
template <class TPixel, unsigned int Dimension>
OrientedRASImage<TPixel, Dimension>::~OrientedRASImage()
{

}
//-------------------------------------------------------------------------------------------------
template <class TPixel, unsigned int Dimension>
typename OrientedRASImage<TPixel, Dimension>::TransformMatrixType
OrientedRASImage<TPixel, Dimension>::
GetVoxelSpaceToRASPhysicalSpaceMatrix()
{
    // Generate intermediate terms
    vnl_matrix<double> m_dir, m_ras_matrix;
    vnl_diag_matrix<double> m_scale, m_lps_to_ras;
    vnl_vector<double> v_origin, v_ras_offset;

    // Compute the matrix
    m_dir = this->GetDirection().GetVnlMatrix();
    m_scale.set(this->GetSpacing().GetVnlVector());
    m_lps_to_ras.set(vnl_vector<double>(Dimension, 1.0));
    m_lps_to_ras[0] = -1;
    m_lps_to_ras[1] = -1;
    m_ras_matrix = m_lps_to_ras * m_dir * m_scale;

    // Compute the vector
    v_origin = this->GetOrigin().GetVnlVector();
    v_ras_offset = m_lps_to_ras * v_origin;

    // Create the larger matrix
    TransformMatrixType mat;
    vnl_vector<double> vcol(Dimension+1, 1.0);
    vcol.update(v_ras_offset);
    mat.SetIdentity();
    mat.GetVnlMatrix().update(m_ras_matrix);
    mat.GetVnlMatrix().set_column(Dimension, vcol);

    return mat;
}
//-------------------------------------------------------------------------------------------------
template <class TPixel, unsigned int Dimension>
void
OrientedRASImage<TPixel, Dimension>::
SetVoxelSpaceToRASPhysicalSpaceMatrix(vnl_matrix<double> mat)
{
    // Generate intermediate terms
    vnl_matrix<double> m_dir,m_dist;
    vnl_diag_matrix<double> m_ras_to_lps, m_scale;
    vnl_vector<double> v_origin ;
    vnl_vector<double> m_spacing(Dimension, 0.0);

    // Get the dim x dim submatrix from mat
    vnl_matrix<double> smat(Dimension,Dimension,0.0);
    for (size_t i=0; i< Dimension; i++)
    {
        for (size_t j=0; j< Dimension; j++)
        {
            smat[i][j] = mat[i][j];
        }
    }
    //smat = mat.get_n_rows(0, Dimension).get_n_columns(0, Dimension);
    // Get the origin
    m_ras_to_lps.set(vnl_vector<double>(Dimension, 1.0));
    m_ras_to_lps[0] = -1;
    m_ras_to_lps[1] = -1;
    vnl_vector<double> v_ras_offset(Dimension,0.0);
    v_ras_offset.fill(0.0);
    for (size_t i=0; i< Dimension; i++)
    {
        v_ras_offset[i] = mat[i][Dimension];
    }
    v_origin = m_ras_to_lps * v_ras_offset;

    // Get the Spacing
    // First, create a matrix of the form [1 0 0; 0 1 0; 0 0 1; 0 0 0] to get distances between consecutive voxels
    // along each axis. When RAS mat will be applied to this matrix, we'll have 3 distance vectors
    vnl_diag_matrix<double> offsetmat(Dimension+1, Dimension);
    offsetmat.fill(0.0);
    for (size_t i=0; i < Dimension+1; i++)
    {
        offsetmat[i]=1.0;
    }
    m_dist = mat * offsetmat;
    // Then compute magnitude of the distance vectors, that's our spacing
    for (size_t i=0; i< Dimension; i++)
    {
        vnl_vector<double> distcol(m_dist.get_column(i));
        m_spacing[i] = distcol.magnitude();
    }
    m_scale.set(m_spacing);

    // Get the direction
    m_scale.invert_in_place();
    m_dir = m_ras_to_lps * smat * m_scale;

    // Set everything
    itk::Matrix<double, Dimension, Dimension> dir;
    dir.SetIdentity();

    for (size_t i=0; i< Dimension; i++)
    {
        for (size_t j=0; j< Dimension; j++)
        {
            dir[i][j] = m_dir[i][j];
        }
    }

    this->SetDirection(dir);
    double origin[Dimension];
    for (size_t i=0; i< Dimension; i++)
    {
        origin[i] = v_origin[i];
    }

    this->SetOrigin(origin);
    double spacing[Dimension];

    for (size_t i=0; i< Dimension; i++)
    {
        spacing[i] = m_spacing[i];
    }

    this->SetSpacing(spacing);
}
//-------------------------------------------------------------------------------------------------
template <class TPixel, unsigned int Dimension>
typename OrientedRASImage<TPixel, Dimension>::TransformMatrixType
OrientedRASImage<TPixel, Dimension>::
GetSpacingOriginPhysicalSpaceToRASPhysicalSpaceMatrix()
{
    TransformMatrixType mat;
    mat.SetIdentity();

    for(size_t i = 0; i < Dimension; i++)
    {
        double ras_flip = (i < 2) ? -1 : 1;
        mat[i][Dimension] = ras_flip * this->GetOrigin()[i];
        for(size_t j = 0; j < Dimension; j++)
        {
            mat[i][j] = ras_flip * this->GetDirection()(i,j) * this->GetSpacing()[i];
            mat[i][Dimension] -= ras_flip * this->GetDirection()(i,j) * this->GetOrigin()[i];
        }
    }

    return mat;
}
//-------------------------------------------------------------------------------------------------
template <class TPixel, unsigned int Dimension>
void
OrientedRASImage<TPixel, Dimension>::
PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf( os, indent );
}
//-------------------------------------------------------------------------------------------------
}

#endif
