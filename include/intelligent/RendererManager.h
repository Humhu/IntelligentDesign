#ifndef _RENDERER_MANAGER_H_
#define _RENDERER_MANAGER_H_

#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkXRenderWindowInteractor.h>
#include <vtkActor.h>
#include <vtkMapper.h>
#include <vtkAlgorithm.h>
#include <vtkCommand.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>

#include <boost/variant.hpp>
#include <boost/thread.hpp>

#include <unordered_map>

#include <X11/Intrinsic.h>

namespace intelligent {

	struct Color {
		double red;
		double green;
		double blue;

		Color();
		Color( double r, double g, double b );
	};
	
	enum RenderShaderType {
		RENDER_SHADER_FLAT,
		RENDER_SHADER_GOURAUD,
		RENDER_SHADER_PHONG
	};
	
	enum RenderGeometryRepresentation {
		RENDER_GEOM_POINTS,
		RENDER_GEOM_WIREFRAME,
		RENDER_GEOM_SURFACE
	};
	
	struct RenderRequest {
		unsigned int id; // The ID for the request target
		Color color;
		bool useLighting;
		RenderShaderType shader;
		RenderGeometryRepresentation representation;
		double ambientLighting; // Unused
		double diffuseLighting;
		double specularLighting;
		double specularPower;
		double opacity; // 1.0 opaque, 0.0 transparent
		bool showEdges;
		Color edgeColor;

		RenderRequest();
	};

	struct CubeRenderRequest : public RenderRequest {
		double center[3];
		double lengths[3];
	};

	struct LineRenderRequest : public RenderRequest {	
		double start[3];
		double finish[3];
	};

	// TODO Plane has different methods for rotating/shifting
	struct PlaneRenderRequest : public RenderRequest {
		double center[3];
		double lengths[2];
		double normal[3];
	};

	typedef boost::variant < CubeRenderRequest, LineRenderRequest,
							 PlaneRenderRequest >
			RenderRequestVariant;


	class RendererManager;
			
	class RenderRequestVisitor : public boost::static_visitor<> {
	public:

		RenderRequestVisitor( RendererManager& _manager );

		void operator()( const CubeRenderRequest& request );
		void operator()( const LineRenderRequest& request );
		void operator()( const PlaneRenderRequest& request );
		
	private:

		RendererManager& manager;

		void SetActorProperties( vtkSmartPointer<vtkActor>& actor,
								 const RenderRequest& request );
	};

	class InteractorTimerCallback : public vtkCommand {
	public:

		static InteractorTimerCallback* New();

		void SetCallback( boost::function<void()> callback );
		
		virtual void Execute( vtkObject* caller, unsigned long eventID,
							  void* callData );
		
	private:

		boost::function<void()> callback;
		
	};

	class KeyPressInteractorStyle : public vtkInteractorStyleTrackballCamera {
	public:
		static KeyPressInteractorStyle* New();
		vtkTypeMacro(KeyPressInteractorStyle, vtkInteractorStyleTrackballCamera);
		
		virtual void OnKeyPress();

		void SetCallback( boost::function<void()> _callback );

	private:

		boost::function<void()> callback;
		
	};
	vtkStandardNewMacro(KeyPressInteractorStyle);
	
	class RendererManager {

	friend class RenderRequestVisitor;
	
	public:

		/*! \brief This manager's name. Also the window name. */
		const std::string name;
		
		RendererManager( const std::string& _name );

		void Clear();
		void QueueRenderRequest( RenderRequestVariant& request );

		void SetScreenshotPrefix( const std::string& _prefix );
		void SetScreenshotMagnification( unsigned int mag );
		
	private:

		boost::mutex queueMutex;
		std::deque<RenderRequestVariant> requestQueue;

		// Base rendering objects
		vtkSmartPointer<vtkRenderer> renderer;
		vtkSmartPointer<vtkRenderWindow> renderWindow;
		vtkSmartPointer<vtkXRenderWindowInteractor> renderInteractor;

		// Screenshot objects
		vtkSmartPointer<vtkWindowToImageFilter> windowToImage;
		vtkSmartPointer<vtkPNGWriter> pngWriter;
		vtkSmartPointer<KeyPressInteractorStyle> keypressStyle;
		std::string screenshotPrefix;
		unsigned int screenshotCounter;
		
		XtAppContext xAppContext;
		boost::thread interactorThread;
		
		struct RenderRegistration {
			vtkSmartPointer<vtkAlgorithm> algorithm;
			vtkSmartPointer<vtkMapper> mapper;
			vtkSmartPointer<vtkActor> actor;
		};

		typedef std::unordered_map <unsigned int, RenderRegistration> RegistrationMap;
		RegistrationMap registry;

		void ProcessRequestQueue();
		void InteractorLoop();
		void CaptureScreenshot();
		
	};
}

#endif