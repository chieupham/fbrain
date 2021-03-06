/*==========================================================================
  
  © Université de Strasbourg - Centre National de la Recherche Scientifique
  
  Date: 05/07/2012
  Author(s): Julien Pontabry (pontabry@unistra.fr)
  
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

#ifndef BTK_DIFFUSION_SEQUENCE_FILE_HELPER_H
#define BTK_DIFFUSION_SEQUENCE_FILE_HELPER_H

// STL includes
#include "vector"
#include "string"

// Local includes
#include "btkGradientDirection.h"

namespace btk
{
/**
 * @class DiffusionSequenceFileHelper
 * @brief The DiffusionSequenceFileHelper is a helper class to handle diffusion Sequence
 * @ingroup Tools
 * @author Julien Pontabry
 */
class DiffusionSequenceFileHelper
{
    public:
        /**
         * @brief Read the gradient table of a diffusion sequence.
         * @param gradientTableFileName Filename of the gradient table to read.
         * @return A vector of GradientDirection.
         */
        static std::vector< GradientDirection > &ReadGradientTable(const std::string &gradientTableFileName);

        /**
         * @brief Write the gradient table of a diffusion sequence.
         * @param gradientTable Gradient table to write.
         * @param gradientTableFileName Filename of the gradient table to write.
         */
        static void WriteGradientTable(std::vector< GradientDirection > &gradientTable, const std::string &gradientTableFileName);

        /**
         * @brief Read the b-values of a diffusion sequence.
         * @param bValuesFileName Filename of the b-values file to read.
         * @return A vector of b-values (unsigned short).
         */
        static std::vector< unsigned short > &ReadBValues(const std::string &bValuesFileName);

        /**
         * @brief Write b-values of a diffusion sequence.
         * @param bValues B-values to write.
         * @param bValuesFileName Filename of the b-values to write.
         */
        static void WriteBValues(std::vector< unsigned short > &bValues, const std::string &bValuesFileName);
};

} // namespace btk

#endif // BTK_DIFFUSION_SEQUENCE_FILE_HELPER_H
