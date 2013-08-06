/*==========================================================================

  © Université de Strasbourg - Centre National de la Recherche Scientifique

  Date: 30/07/2013
  Author(s): Aïcha Bentaieb (abentaieb@unistra.fr)

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

/* VTK, BTK, Std Lib Includes */
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkPolyDataReader.h"
#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkCell.h"
#include "vtkVertex.h"
#include "vtkPolyDataWriter.h"
#include "vtkMath.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkProperty.h"

#include "btkCurvatures.h"
#include "btkRegionGrow.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <tclap/CmdLine.h>
#include <vector>
#include <sstream>


// ---------------------------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    try
    {

        TCLAP::CmdLine cmd("Region growing calling btkRegionGrow", ' ', "1.0");
        TCLAP::ValueArg<std::string> inputFile("i", "input_file", "Input mesh triangle file corresponding to curvature mesh (vtk file)", true, "", "string");
        cmd.add(inputFile);
        TCLAP::ValueArg<std::string> seedCell("s","seed_cell","Seed Cell ID used for initialization of the region growing", true, "", "int");
        cmd.add(seedCell);
        TCLAP::ValueArg<int> numberOfPoints("n", "nb_points", "Total number of points to select, default = 20 points", false, 20, "integer");
        cmd.add(numberOfPoints);
        TCLAP::ValueArg<std::string> outputFile("o", "output_file", "Output mesh triangle file = selected points (vtk file)", true, "", "string");
        cmd.add(outputFile);
        cmd.parse(argc,argv);
        std::string input_file = inputFile.getValue();
        std::string seed_cell = seedCell.getValue();
        int nb_points = numberOfPoints.getValue();
        std::string output_file = outputFile.getValue();

        // read
        vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
        reader->SetFileName(input_file.c_str());
        reader->Update();
        vtkSmartPointer<vtkPolyData> data = vtkSmartPointer<vtkPolyData>::New();
        data = reader->GetOutput();
        data->Update();


        // region growing
        data->BuildLinks();
        vtkIdType sp = 0;
        std::istringstream buffer(seed_cell);
        buffer >> sp ;
        vtkIdType  npts, *choosedPoints;

        vtkIdType seedPoint;
        std::cout<<"Seed Cell ID = "<<seed_cell<<std::endl;
        data->GetCellPoints(sp,npts,choosedPoints);
        seedPoint = choosedPoints[1];
        std::cout<<"Start region grow..."<<std::endl;
        std::vector<vtkIdType> sillon;
        btkRegionGrow *region = btkRegionGrow::New();
        region->setPolydata(data);
        region->setSeedPoint(seedPoint);
        region->setNumberOfPoints(nb_points);
        region->Update();
        sillon = region->GetOutput();

        if (nb_points > sillon.size())
            std::cout<< "Couldnt find more than "<<sillon.size()<< " points in the region of interest."<<std::endl;


        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();// geometry of the point
        vtkSmartPointer<vtkVertex> vertex = vtkSmartPointer<vtkVertex>::New();
        vtkSmartPointer<vtkCellArray> verticies = vtkSmartPointer<vtkCellArray>::New();// topology

        double pointCoords[3];

        for(int i=0; i<sillon.size(); i++)
        {
            data->GetPoint(sillon[i],pointCoords);
            points->InsertNextPoint(pointCoords);
        }
        vertex->GetPointIds()->SetNumberOfIds(points->GetNumberOfPoints());
        for(int i=0; i<points->GetNumberOfPoints(); i++)
        {
            vertex->GetPointIds()->SetId(i,i);
        }
        verticies->InsertNextCell(vertex);

        vtkSmartPointer<vtkPolyData> output = vtkSmartPointer<vtkPolyData>::New();
        output->SetPoints(points);
        output->SetVerts(verticies);

        std::cout <<"n° of verts: "<< output->GetNumberOfPoints()<<std::endl;
        std::cout <<"n° of points: "<< output->GetNumberOfVerts()<<std::endl;

        // write
        vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
        writer->SetInput(output);
        writer->SetFileName(output_file.c_str());
        writer->Write();

        // Visualize the polydata with the resulting points from region grow (red points)
        vtkSmartPointer<vtkPolyDataMapper> mappercurv = vtkSmartPointer<vtkPolyDataMapper>::New();
        mappercurv->SetInput(data);
        mappercurv->ScalarVisibilityOff(); // turn off the visibility of the mesh mapper (curv) in order to be able to see the ROI only colored
        vtkSmartPointer<vtkActor> actorcurv = vtkSmartPointer<vtkActor>::New();
        actorcurv->SetMapper(mappercurv);

        vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
        vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
        renderWindow->AddRenderer(renderer);
        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
        renderWindowInteractor->SetRenderWindow(renderWindow);
        renderer->AddActor(actorcurv);

        vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInput(output);
        mapper->ScalarVisibilityOff();

        vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        actor->GetProperty()->SetPointSize(5);
        actor->GetProperty()->SetColor(1,0,0);
        renderer->AddActor(actor);
        renderWindow->Render();
        renderWindowInteractor->Start();

        return EXIT_SUCCESS;

    }
    catch(TCLAP::ArgException &e) // catch any exception
    {std::cerr << "error: " << e.error() << "for arg " << e.argId() << std::endl;}

}
