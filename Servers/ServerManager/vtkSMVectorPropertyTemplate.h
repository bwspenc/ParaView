/*=========================================================================

  Program:   ParaView
  Module:    $RCSfile$

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSMVectorPropertyTemplate
// .SECTION Description
//

#ifndef __vtkSMVectorPropertyTemplate_h
#define __vtkSMVectorPropertyTemplate_h

#include <assert.h>
#include <vtkstd/vector>
#include <vtkstd/algorithm>
class vtkSMProperty;

template <class T>
class vtkSMVectorPropertyTemplate
{
  vtkSMProperty* Property;
public:
  vtkstd::vector<T> Values;
  vtkstd::vector<T> UncheckedValues;
  vtkstd::vector<T> DefaultValues; // Values set in the XML configuration.
  bool DefaultsValid;
  bool Initialized;

  //---------------------------------------------------------------------------
  vtkSMVectorPropertyTemplate(vtkSMProperty* property)
    {
    this->Property = property;
    this->DefaultsValid = false;
    this->Initialized = false;
    }

  //---------------------------------------------------------------------------
  void UpdateDefaultValues()
    {
    this->DefaultValues.clear();
    this->DefaultValues.insert(this->DefaultValues.end(),
      this->Values.begin(), this->Values.end());
    this->DefaultsValid = true;
    }

  //---------------------------------------------------------------------------
  void SetNumberOfUncheckedElements(unsigned int num)
    {
    this->UncheckedValues.resize(num, 0);
    }

  //---------------------------------------------------------------------------
  unsigned int GetNumberOfUncheckedElements()
    {
    return static_cast<unsigned int>(this->UncheckedValues.size());
    }

  //---------------------------------------------------------------------------
  unsigned int GetNumberOfElements()
    {
    return static_cast<unsigned int>(this->Values.size());
    }

  //---------------------------------------------------------------------------
  void SetNumberOfElements(unsigned int num)
    {
    if (num == this->Values.size())
      {
      return;
      }
    this->Values.resize(num, 0);
    this->UncheckedValues.resize(num, 0);
    if (num == 0)
      {
      // If num == 0, then we already have the intialized values (so to speak).
      this->Initialized = true;
      }
    else
      {
      this->Initialized = false;
      }
    this->Property->Modified();
    }

  //---------------------------------------------------------------------------
  T& GetElement(unsigned int idx)
    {
    assert(idx < this->Values.size());
    return this->Values[idx];
    }

  //---------------------------------------------------------------------------
  // seems weird that this idx is "int".
  T& GetDefaultValue(int idx)
    {
    if (idx >= 0 && idx < static_cast<int>(this->DefaultValues.size()))
      {
      return this->DefaultValues[idx];
      }

    static T empty_value = T();
    return empty_value;
    }

  //---------------------------------------------------------------------------
  T* GetElements()
    {
    return (this->Values.size() > 0)?  &this->Values[0] : NULL;
    }

  //---------------------------------------------------------------------------
  T& GetUncheckedElement(unsigned int idx)
    {
    assert(idx < this->UncheckedValues.size());
    return this->UncheckedValues[idx];
    }

  //---------------------------------------------------------------------------
  void SetUncheckedElement(unsigned int idx, T value)
    {
    if (idx >= this->GetNumberOfUncheckedElements())
      {
      this->SetNumberOfUncheckedElements(idx+1);
      }
    this->UncheckedValues[idx] = value;
    }

  //---------------------------------------------------------------------------
  int SetElement(unsigned int idx, T value)
    {
    unsigned int numElems = this->GetNumberOfElements();

    if (this->Initialized && idx < numElems && value == this->GetElement(idx))
      {
      return 1;
      }

    if ( vtkSMProperty::GetCheckDomains() )
      {
      this->UncheckedValues = this->Values;
      this->SetUncheckedElement(idx, value);
      if (!this->Property->IsInDomains())
        {
        this->SetNumberOfUncheckedElements(numElems);
        return 0;
        }
      }

    if (idx >= numElems)
      {
      this->SetNumberOfElements(idx+1);
      }
    this->Values[idx] = value;
    // Make sure to initialize BEFORE Modified() is called. Otherwise,
    // the value would not be pushed.
    this->Initialized = true;
    this->Property->Modified();
    return 1;
    }

  //---------------------------------------------------------------------------
  int SetElements(const T* values)
    {
    return this->SetElements(values, this->GetNumberOfElements());
    }

  //---------------------------------------------------------------------------
  int SetElements(const T* values, unsigned int numValues)
    {
    bool modified = false;
    unsigned int numArgs = this->GetNumberOfElements();
    if (numArgs != numValues)
      {
      this->Values.resize(numValues);
      this->UncheckedValues.resize(numValues);
      numArgs = numValues;
      modified = true;
      }
    else
      {
      modified = !vtkstd::equal(this->Values.begin(), this->Values.end(), values);
      }
    if (!modified && this->Initialized)
      {
      return 1;
      }

    if ( vtkSMProperty::GetCheckDomains() )
      {
      vtkstd::copy(values, values+numArgs, this->UncheckedValues.begin());
      if (!this->Property->IsInDomains())
        {
        return 0;
        }
      }

    vtkstd::copy(values, values+numArgs, this->Values.begin());
    this->Initialized = true;
    this->Property->Modified();
    return 1;
    }


  //---------------------------------------------------------------------------
  void Copy(vtkSMVectorPropertyTemplate<T>* dsrc)
    {
    if (dsrc && dsrc->Initialized)
      {
      bool modified = false;

      if (this->Values != dsrc->Values)
        {
        this->Values = dsrc->Values;
        modified = true;
        }
      // If we were not initialized, we are now modified even if the value
      // did not change
      modified = modified || !this->Initialized;
      this->Initialized = true;

      this->UncheckedValues = dsrc->UncheckedValues;
      if (modified)
        {
        this->Property->Modified();
        }
      }
    }

  //---------------------------------------------------------------------------
  void ResetToDefaultInternal()
    {
    if (this->DefaultsValid && this->DefaultValues != this->Values)
      {
      this->Values = this->DefaultValues;
      // Make sure to initialize BEFORE Modified() is called. Otherwise,
      // the value would not be pushed.
      this->Initialized = true;
      this->Property->Modified();
      }
    }


};
#endif