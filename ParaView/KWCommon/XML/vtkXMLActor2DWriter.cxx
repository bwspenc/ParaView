/*=========================================================================

Copyright (c) 1998-2003 Kitware Inc. 469 Clifton Corporate Parkway,
Clifton Park, NY, 12065, USA.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * Neither the name of Kitware nor the names of any contributors may be used
   to endorse or promote products derived from this software without specific
   prior written permission.

 * Modified source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include "vtkXMLActor2DWriter.h"

#include "vtkActor2D.h"
#include "vtkObjectFactory.h"
#include "vtkProperty2D.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLProperty2DWriter.h"

vtkStandardNewMacro(vtkXMLActor2DWriter);
vtkCxxRevisionMacro(vtkXMLActor2DWriter, "1.4");

//----------------------------------------------------------------------------
char* vtkXMLActor2DWriter::GetRootElementName()
{
  return "Actor2D";
}

//----------------------------------------------------------------------------
char* vtkXMLActor2DWriter::GetPropertyElementName()
{
  return "Property";
}

//----------------------------------------------------------------------------
vtkXMLActor2DWriter::vtkXMLActor2DWriter()
{
  this->DoNotOutputPosition = 0;
}

//----------------------------------------------------------------------------
int vtkXMLActor2DWriter::AddAttributes(vtkXMLDataElement *elem)
{
  if (!this->Superclass::AddAttributes(elem))
    {
    return 0;
    }

  vtkActor2D *obj = vtkActor2D::SafeDownCast(this->Object);
  if (!obj)
    {
    vtkWarningMacro(<< "The Actor2D is not set!");
    return 0;
    }

  elem->SetIntAttribute("LayerNumber", obj->GetLayerNumber());

  if (!this->DoNotOutputPosition)
    {
    vtkCoordinate *coord = obj->GetPositionCoordinate();
    if (coord)
      {
      int sys = coord->GetCoordinateSystem();
      coord->SetCoordinateSystemToNormalizedViewport();
      elem->SetVectorAttribute("Position", 2, coord->GetValue());
      coord->SetCoordinateSystem(sys);
      }

    coord = obj->GetPosition2Coordinate();
    if (coord)
      {
      int sys = coord->GetCoordinateSystem();
      coord->SetCoordinateSystemToNormalizedViewport();
      elem->SetVectorAttribute("Position2", 2, coord->GetValue());
      coord->SetCoordinateSystem(sys);
      }
    }

  return 1;
}

//----------------------------------------------------------------------------
int vtkXMLActor2DWriter::AddNestedElements(vtkXMLDataElement *elem)
{
  if (!this->Superclass::AddNestedElements(elem))
    {
    return 0;
    }

  vtkActor2D *obj = vtkActor2D::SafeDownCast(this->Object);
  if (!obj)
    {
    vtkWarningMacro(<< "The Actor2D is not set!");
    return 0;
    }

  // Property2D

  vtkProperty2D *prop2d = obj->GetProperty();
  if (prop2d)
    {
    vtkXMLProperty2DWriter *xmlw = vtkXMLProperty2DWriter::New();
    xmlw->SetObject(prop2d);
    xmlw->CreateInNestedElement(elem, this->GetPropertyElementName());
    xmlw->Delete();
    }
 
  return 1;
}

//----------------------------------------------------------------------------
void vtkXMLActor2DWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "DoNotOutputPosition: "
     << (this->DoNotOutputPosition ? "On" : "Off") << endl;
}