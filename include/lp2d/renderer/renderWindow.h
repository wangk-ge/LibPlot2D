/*=============================================================================
                                   LibPlot2D
                       Copyright Kerry R. Loux 2011-2016

                  This code is licensed under the GPLv2 License
                    (http://opensource.org/licenses/GPL-2.0).
=============================================================================*/

// File:  renderWindow.h
// Date:  5/14/2009
// Auth:  K. Loux
// Desc:  Class for creating OpenGL scenes, derived from wxGLCanvas.  Contains
//        event handlers for various mouse and keyboard interactions.  All
//        object in the scene must be added to the PrimitivesList in order to
//        be drawn.  Objects in the PrimitivesList become managed by this
//        object and are deleted automatically.

#ifndef RENDER_WINDOW_H_
#define RENDER_WINDOW_H_

// Local headers
#include "lp2d/utilities/managedList.h"
#include "lp2d/renderer/primitives/primitive.h"

// Eigen headers
#include <Eigen/Eigen>

// wxWidgets headers
#include <wx/wx.h>
#include <wx/glcanvas.h>

// Standard C++ headers
#include <memory>

namespace LibPlot2D
{

/// Class for creating OpenGL scenes.  Includes event handlers for various
/// mouse and keyboard interactions.
class RenderWindow : public wxGLCanvas
{
public:
	/// Constructor.
	///
	/// \param parent   Reference to owning window.
	/// \param id       Window id.
	/// \param attr     Requested OpenGL canvas/context attribute list.
	/// \param position Position of this window within the parent window.
	/// \param size	    Initial size of this window.
	/// \param style    Style flags.
	RenderWindow(wxWindow &parent, wxWindowID id, const wxGLAttributes& attr,
		const wxPoint& position, const wxSize& size, long style = 0);
	~RenderWindow() = default;

	/// Initializes this object to prepare for rendering.  Must be called
	/// immediately after creation.
	void Initialize();

	/// Sets the camera position and orientation.
	///
	/// \param position    Location of the camera's "eye."
	/// \param lookAt      Point at which the camera is pointed.
	/// \param upDirection Direction which will be up in the rendered image.
	void SetCameraView(const Eigen::Vector3d &position,
		const Eigen::Vector3d &lookAt, const Eigen::Vector3d &upDirection);

	/// @{
	
	/// Transforms between the model coordinate system and the view (OpenGL)
	/// coordinate system.
	///
	/// \param modelVector Vector in model coordinates.
	///
	/// \returns Vector in view coordinates.
	Eigen::Vector3d TransformToView(const Eigen::Vector3d &modelVector) const;

	/// Transforms between the view (OpenGL) coordinate system and the model
	/// coordinate system.
	///
	/// \param viewVector Vector in view coordinates.
	///
	/// \returns Vector in model coordinates.
	Eigen::Vector3d TransformToModel(const Eigen::Vector3d &viewVector) const;

	/// @}

	/// Queries the current camera position.
	/// \returns The current camera position.
	Eigen::Vector3d GetCameraPosition() const;

	/// Sets the viewing frustum to match the current size of the window.
	void AutoSetFrustum();

	/// Adds objects to the list of primitives to render.  In addition to
	/// rendering the specified object, we also take ownership of the added
	/// primitives.
	///
	/// \param toAdd Object to add to the scene.
	inline void AddActor(std::unique_ptr<Primitive> toAdd)
	{ mPrimitiveList.Add(std::move(toAdd)); mModified = true; }

	/// Removes the specified object from the primitives list.
	///
	/// \param toRemove Pointer to object to be removed.
	///
	/// \returns True if the specified object was found and removed.
	bool RemoveActor(Primitive *toRemove);

	/// \name Setters
	/// @{

	inline void SetWireFrame(const bool& wireFrame)
	{ mWireFrame = wireFrame; mModified = true; }
	void SetViewOrthogonal(const bool& viewOrthogonal);

	inline void SetTopMinusBottom(const double& topMinusBottom)
	{ mTopMinusBottom = topMinusBottom; mModified = true; }
	inline void SetAspectRatio(const double& aspectRatio)
	{ mAspectRatio = aspectRatio; mModified = true; }
	inline void SetNearClip(const double& nearClip)
	{ mNearClip = nearClip; mModified = true; }
	inline void SetFarClip(const double& farClip)
	{ mFarClip = farClip; mModified = true; }
	inline void SetView3D(const bool& view3D)
	{ mView3D = view3D; mModified = true; }

	virtual void SetBackgroundColor(const Color& backgroundColor)
	{ mBackgroundColor = backgroundColor; mModified = true; }

	/// @}

	/// \name Getters
	/// @{

	inline Color GetBackgroundColor() { return mBackgroundColor; }

	inline bool GetWireFrame() const { return mWireFrame; }
	inline bool GetViewOrthogonal() const { return mViewOrthogonal; }
	inline bool GetView3D() const { return mView3D; }

	inline double GetAspectRatio() const { return mAspectRatio; }

	/// @}

	/// @{

	/// Gets a string describing any existing OpenGL errors.
	/// \returns A string describing any existing OpenGL errors.
	static wxString GetGLError();

	/// Gets a string describing the specified OpenGL error.
	///
	/// \param e Error code.
	///
	/// \returns Description of the specified error code.
	static wxString GetGLError(const GLint& e);

	/// @}

	/// Checks to see if an OpenGL error exists.
	/// \returns True if an error exists.
	static bool GLHasError();

	/// Writes the current image to file.
	///
	/// \param pathAndFileName Location to write the file.
	///
	/// \returns True if the file was successfully written.
	bool WriteImageToFile(wxString pathAndFileName) const;

	/// Gets an image of the current rendered scene.
	/// \returns An image object representing the current scene.
	virtual wxImage GetImage() const;

	/// Determines if a particular primitive is in the scene owned by this
	/// object.
	///
	/// \param pickedObject Pointer to the queried object.
	///
	/// \returns True if the \p pickedObject is owned by this.
	bool IsThisRendererSelected(const Primitive *pickedObject) const;

	/// Sets a flag indicating that the primitives must be sorted by alpha
	/// prior to rendering.
	void SetNeedAlphaSort() { mNeedAlphaSort = true; }

	/// Sets a flag indicating that the primitives must be sorted by drawOrder
	/// prior to rendering.
	void SetNeedOrderSort() { mNeedOrderSort = true; }

	/// Compiles the specified shader.
	///
	/// \param type           Type of shader to build.
	/// \param shaderContents Shader instructions.
	///
	/// \returns Index for compiled shader.
	static GLuint CreateShader(const GLenum& type,
		const std::string& shaderContents);

	/// Creates the program using the specified shaders.
	///
	/// \param shaderList List of shaders to include in the program.
	///
	/// \returns Index for the program.
	static GLuint CreateProgram(const std::vector<GLuint>& shaderList);

	/// Applies a small shift to the modelview matrix to enable exact
	/// pixelization.
	void ShiftForExactPixelization();
	void UseDefaultProgram() const;///< Sets the default program as active.

	/// Gets the location of the position attribute within the current program.
	/// \returns The location of the position attribute.
	GLuint GetPositionLocation() const { return mPositionAttributeLocation; }

	/// Gets the location of the color attribute within the current program.
	/// \returns The location of the color attribute.
	GLuint GetColorLocation() const { return mColorAttributeLocation; }

	/// Gets the expected vertex dimension for this object.  Use this to ensure
	/// compatibility with the default program when building vertex array
	/// objects.
	/// \returns The dimension of a vertex in the default program.
	virtual unsigned int GetVertexDimension() const { return 4; }

	/// \name Matrix manipulation methods
	/// @{

	static void Translate(Eigen::Matrix4d& m, const Eigen::Vector3d& v);
	static void Rotate(Eigen::Matrix4d& m, const double& angle,
		Eigen::Vector3d axis);
	static void Scale(Eigen::Matrix4d& m, const Eigen::Vector3d& v);

	/// @}

	/// Structre for storing information about shader programs.
	struct ShaderInfo
	{
		GLuint programId;///< OpenGL id for the shader.

		/// Flag indicating whether or not a projection matrix is required.
		bool needsProjection;

		/// Location of the projection matrix within the shader.
		GLuint projectionLocation;

		/// Flag indicating whether or not a modelview matrix is required.
		bool needsModelview;

		/// Location of the modelview matrix within the shader.
		GLuint modelViewLocation;
	};

	/// Adds the specified shader to our list of programs.
	///
	/// \param shader Program to add.
	void AddShader(const ShaderInfo& shader);

	/// Sends the specified matrix to the specified uniform location within the
	/// current program.
	///
	/// \param m        Matrix to send.
	/// \param location Location within the program.
	static void SendUniformMatrix(const Eigen::Matrix4d& m,
		const GLuint& location);

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

protected:
	bool mView3D = true;///< Flag indicating whether or not scene is 3D.
	bool mModified = true;///< Flag indicatin whether or not scene has changed.
	bool mSizeUpdateRequired = true;///< Flag indicating that size has changed.

	ManagedList<Primitive> mPrimitiveList;///< List of objects to be rendered.

	/// Stores the current location of the mouse cursor.
	///
	/// \param event Mouse location information.
	void StoreMousePosition(wxMouseEvent &event);
	long mLastMousePosition[2];///< Last known location of the mouse.

	/// Enumeration of interaction types supported by this object.
	enum class Interaction
	{
		DollyDrag,///< Zooming action vi mouse drag.
		DollyWheel,///< Zooming action vi mouse wheel.
		Pan,///< Translation of the scene.
		Rotate///< Rotation of the scene.
	};

	/// @{
	
	/// Determines the type of interaction occurring (if any).
	///
	/// \param event             Information about recent mouse inputs.
	/// \param interaction [out] Type of interaction detected.
	///
	/// \returns True if an interaction is occurring.
	bool Determine2DInteraction(const wxMouseEvent &event,
		Interaction &interaction) const;

	/// Determines the type of interaction occurring (if any).
	///
	/// \param event             Information about recent mouse inputs.
	/// \param interaction [out] Type of interaction detected.
	///
	/// \returns True if an interaction is occurring.
	bool Determine3DInteraction(const wxMouseEvent &event,
		Interaction &interaction) const;

	/// @}

	/// Flag indicating whether or not we should select a new focal point for
	/// the interactions.
	bool mIsInteracting = false;

	/// Converts from internal matrix representation to a format that can be
	/// passed to OpenGL.
	///
	/// \param matrix   Matrix in internal representation.
	/// \param gl [out] OpenGL matrix represenation.
	static void ConvertMatrixToGL(const Eigen::Matrix4d& matrix, float gl[]);

	/// Converts from OpenGL matrix represenation to our internal type.
	///
	/// \param gl           OpenGL matrix represenation.
	/// \param matrix [out] Matrix in internal representation.
	static void ConvertGLToMatrix(const float gl[], Eigen::Matrix4d& matrix);

	void Initialize2D();///< Initializes this object for 2D rendering.
	void Initialize3D();///< Initializes this object for 3D rendering.

	/// @{
	
	/// Creates the appropriate projection matrix.
	/// \returns Projection matrix.
	Eigen::Matrix4d Generate2DProjectionMatrix() const;

	/// Creates the appropriate projection matrix.
	/// \returns Projection matrix.
	Eigen::Matrix4d Generate3DProjectionMatrix() const;

	/// @}

	bool mModelviewModified = true;///< Flag indicating status of modelview.
	Eigen::Matrix4d mModelviewMatrix;///< Modelview matrix.

	std::vector<ShaderInfo> mShaders;///< List of available shader programs.

	/// Gets the default vertex shader for this object.
	/// \returns Default vertex shader.
	virtual std::string GetDefaultVertexShader() const
	{ return mDefaultVertexShader; }

	/// Gets the default fragment shader for this object.
	/// \returns Default fragment shader.
	virtual std::string GetDefaultFragmentShader() const
	{ return mDefaultFragmentShader; }

	/// Checks to see if this object has a geometry shader.
	/// \returns True if this object has a geometry shader.
	virtual bool HasGeometryShader() const { return false; }

	/// Gets the default geometry shader for this object.
	/// \returns Default geometry shader.
	virtual std::string GetDefaultGeometryShader() const
	{ return std::string(); }

	/// Assigns required indicies and/or values for uniforms within the default shader.
	virtual void AssignDefaultUniforms(ShaderInfo& shader);

	/// Flag indicating whether or not this object saw a left-button-down
	/// event.  Tells us if we should respond to left-button-up or drag events.
	bool mObservedLeftButtonDown = false;

	DECLARE_EVENT_TABLE()

private:
	std::unique_ptr<wxGLContext> mContext;
	wxGLContext* GetContext();

	static const double mExactPixelShift;

	// Flags describing the options for this object's functionality
	bool mWireFrame = false;
	bool mViewOrthogonal = false;

	// The parameters that describe the viewing frustum
	double mTopMinusBottom = 100.0;// in model-space units
	double mAspectRatio;
	double mNearClip = 1.0;
	double mFarClip = 500.0;

	Color mBackgroundColor;

	static bool AlphaSortPredicate(const std::unique_ptr<Primitive>& p1,
		const std::unique_ptr<Primitive>& p2);
	static bool OrderSortPredicate(const std::unique_ptr<Primitive>& p1,
		const std::unique_ptr<Primitive>& p2);

	bool mNeedAlphaSort = true;
	bool mNeedOrderSort = true;

	// Event handlers-----------------------------------------------------
	// Interactor events
	virtual void OnMouseWheelEvent(wxMouseEvent &event);
	virtual void OnMouseMoveEvent(wxMouseEvent &event);
	virtual void OnMouseUpEvent(wxMouseEvent &event);
	virtual void OnMouseDownEvent(wxMouseEvent& event);

	// Window events
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnEnterWindow(wxMouseEvent &event);
	// End event handlers-------------------------------------------------

	void Render();

	// Performs the computations and transformations associated with the
	// specified interaction
	void PerformInteraction(Interaction interaction, wxMouseEvent &event);

	// The interaction events (called from within the real event handlers)
	void DoRotate(wxMouseEvent &event);
	void DoWheelDolly(wxMouseEvent &event);
	void DoDragDolly(wxMouseEvent &event);
	void DoPan(wxMouseEvent &event);

	// Updates the transformation matrices according to the current modelview
	// matrix
	//void UpdateTransformationMatricies();
	void UpdateModelviewMatrix();

	static const std::string mModelviewName;
	static const std::string mProjectionName;
	static const std::string mPositionName;
	static const std::string mColorName;

	static const std::string mDefaultVertexShader;
	static const std::string mDefaultFragmentShader;

	void BuildShaders();

	GLuint CreateDefaultVertexShader();
	GLuint CreateDefaultFragmentShader();
	GLuint CreateDefaultGeometryShader();

	GLuint mPositionAttributeLocation;
	GLuint mColorAttributeLocation;

	Eigen::Vector3d mFocalPoint;

	void DoResize();

	bool mGlewInitialized = false;
};

}// namespace LibPlot2D

#endif// RENDER_WINDOW_H_
