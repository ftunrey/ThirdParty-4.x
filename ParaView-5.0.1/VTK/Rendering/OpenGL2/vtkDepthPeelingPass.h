/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkDepthPeelingPass.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkDepthPeelingPass - Implement an Order Independent Transparency
// render pass.
// .SECTION Description
// Render the translucent polygonal geometry of a scene without sorting
// polygons in the view direction.
//
// This pass expects an initialized depth buffer and color buffer.
// Initialized buffers means they have been cleared with farest z-value and
// background color/gradient/transparent color.
// An opaque pass may have been performed right after the initialization.
//
// The depth peeling algorithm works by rendering the translucent polygonal
// geometry multiple times (once for each peel). The actually rendering of
// the translucent polygonal geometry is performed by its delegate
// TranslucentPass. This delegate is therefore used multiple times.
//
// Its delegate is usually set to a vtkTranslucentPass.
//
// .SECTION See Also
// vtkRenderPass, vtkTranslucentPass

#ifndef vtkDepthPeelingPass_h
#define vtkDepthPeelingPass_h

#include "vtkRenderingOpenGL2Module.h" // For export macro
#include "vtkRenderPass.h"
#include <vector>  // STL Header

class vtkTextureObject;
class vtkOpenGLRenderWindow;
class vtkInformationIntegerKey;
class vtkInformationIntegerVectorKey;
class vtkOpenGLHelper;

class VTKRENDERINGOPENGL2_EXPORT vtkDepthPeelingPass : public vtkRenderPass
{
public:
  static vtkDepthPeelingPass *New();
  vtkTypeMacro(vtkDepthPeelingPass,vtkRenderPass);
  void PrintSelf(ostream& os, vtkIndent indent);

  //BTX
  // Description:
  // Perform rendering according to a render state \p s.
  // \pre s_exists: s!=0
  virtual void Render(const vtkRenderState *s);
  //ETX

  // Description:
  // Release graphics resources and ask components to release their own
  // resources.
  // \pre w_exists: w!=0
  void ReleaseGraphicsResources(vtkWindow *w);

  // Description:
  // Delegate for rendering the translucent polygonal geometry.
  // If it is NULL, nothing will be rendered and a warning will be emitted.
  // It is usually set to a vtkTranslucentPass.
  // Initial value is a NULL pointer.
  vtkGetObjectMacro(TranslucentPass,vtkRenderPass);
  virtual void SetTranslucentPass(vtkRenderPass *translucentPass);

  // Description:
  // In case of use of depth peeling technique for rendering translucent
  // material, define the threshold under which the algorithm stops to
  // iterate over peel layers. This is the ratio of the number of pixels
  // that have been touched by the last layer over the total number of pixels
  // of the viewport area.
  // Initial value is 0.0, meaning rendering have to be exact. Greater values
  // may speed-up the rendering with small impact on the quality.
  vtkSetClampMacro(OcclusionRatio,double,0.0,0.5);
  vtkGetMacro(OcclusionRatio,double);

  // Description:
  // In case of depth peeling, define the maximum number of peeling layers.
  // Initial value is 4. A special value of 0 means no maximum limit.
  // It has to be a positive value.
  vtkSetMacro(MaximumNumberOfPeels,int);
  vtkGetMacro(MaximumNumberOfPeels,int);

  // Description:
  // Is rendering at translucent geometry stage using depth peeling and
  // rendering a layer other than the first one? (Boolean value)
  // If so, the uniform variables UseTexture and Texture can be set.
  // (Used by vtkOpenGLProperty or vtkOpenGLTexture)
//  int GetDepthPeelingHigherLayer();

  // Description:
  // Required Key Indicating the texture unit for the opaque z texture unit
  static vtkInformationIntegerKey *OpaqueZTextureUnit();

  // Description:
  // Required Key Indicating the texture unit for the translucent z texture unit
  static vtkInformationIntegerKey *TranslucentZTextureUnit();

  // Description:
  // Required Key Indicating the size of the destination
  static vtkInformationIntegerVectorKey *DestinationSize();

 protected:
  // Description:
  // Default constructor. TranslucentPass is set to NULL.
  vtkDepthPeelingPass();

  // Description:
  // Destructor.
  virtual ~vtkDepthPeelingPass();

  vtkRenderPass *TranslucentPass;
  vtkTimeStamp CheckTime;
  bool IsSupported;

  // Description:
  // Cache viewport values for depth peeling.
  int ViewportX;
  int ViewportY;
  int ViewportWidth;
  int ViewportHeight;

  // Description:
  // In case of use of depth peeling technique for rendering translucent
  // material, define the threshold under which the algorithm stops to
  // iterate over peel layers. This is the ratio of the number of pixels
  // that have been touched by the last layer over the total number of pixels
  // of the viewport area.
  // Initial value is 0.0, meaning rendering have to be exact. Greater values
  // may speed-up the rendering with small impact on the quality.
  double OcclusionRatio;

  // Description:
  // In case of depth peeling, define the maximum number of peeling layers.
  // Initial value is 4. A special value of 0 means no maximum limit.
  // It has to be a positive value.
  int MaximumNumberOfPeels;

  // Is rendering at translucent geometry stage using depth peeling and
  // rendering a layer other than the first one? (Boolean value)
  // If so, the uniform variables UseTexture and Texture can be set.
  // (Used by vtkOpenGLProperty or vtkOpenGLTexture)
  int DepthPeelingHigherLayer;

  vtkOpenGLHelper *FinalBlendProgram;
  vtkOpenGLHelper *IntermediateBlendProgram;

  vtkTextureObject *OpaqueZTexture;
  vtkTextureObject *OpaqueRGBATexture;
  vtkTextureObject *TranslucentRGBATexture;
  vtkTextureObject *TranslucentZTexture;
  vtkTextureObject *CurrentRGBATexture;
  std::vector<float> *DepthZData;

  void BlendIntermediatePeels(vtkOpenGLRenderWindow *renWin, bool);
  void BlendFinalPeel(vtkOpenGLRenderWindow *renWin);

 private:
  vtkDepthPeelingPass(const vtkDepthPeelingPass&);  // Not implemented.
  void operator=(const vtkDepthPeelingPass&);  // Not implemented.
};

#endif
