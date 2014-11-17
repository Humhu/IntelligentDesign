#include "intelligent/RendererManager.h"

#include <vtkArrowSource.h>
#include <vtkTransform.h>
#include <vtkCubeSource.h>
#include <vtkLineSource.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkProperty.h>
#include <vtkMath.h>

#include <stdexcept>

#include <boost/foreach.hpp>

namespace intelligent {

	Color::Color() :
		red( 0 ), green( 0 ), blue( 0 ) {}
		
	Color::Color( double r, double g, double b ) :
		red( r ), green( g ), blue( b ) {}
	
	RenderRequest::RenderRequest() {
		color = Color( 1.0, 1.0, 1.0 );
		useLighting = true;
		shader = RENDER_SHADER_FLAT;
		representation = RENDER_GEOM_SURFACE;
		ambientLighting = 1.0;
		diffuseLighting = 1.0;
		specularLighting = 1.0;
		specularPower = 1.0;
		opacity = 1.0;
		showEdges = true;
		edgeColor = Color( 0.0, 0.0, 0.0 );
	}

	ArrowRenderRequest::ArrowRenderRequest() {
		tipLength = 0.35;
		tipRadius = 0.1;
		tipResolution = 10;
		shaftRadius = 0.03;
		shaftResolution = 10;
	}
	
	RenderRequestVisitor::RenderRequestVisitor( RendererManager& _manager ) :
		manager( _manager ) {}

	// TODO Reduce code duplication in following operator()s
	void RenderRequestVisitor::operator()( const CubeRenderRequest& request ) {
		
		vtkCubeSource* cube;
		vtkSmartPointer<vtkActor> actor;

		vtkSmartPointer<vtkCubeSource> cu = vtkSmartPointer<vtkCubeSource>::New();
		vtkSmartPointer<vtkPolyDataMapper> pdm = vtkSmartPointer<vtkPolyDataMapper>::New();
		actor = vtkSmartPointer<vtkActor>::New();

		pdm->SetInputConnection( cu->GetOutputPort() );
		actor->SetMapper( pdm );
		manager.renderer->AddActor( actor );

		RendererManager::RenderRegistration registration;
		registration.algorithms.push_back( cu );
		registration.mapper = pdm;
		registration.actor = actor;
		manager.registry[ request.id ] = registration;

		cube = cu.GetPointer();

		cube->SetCenter( request.center[0], request.center[1], request.center[2] );
		cube->SetXLength( request.lengths[0] );
		cube->SetYLength( request.lengths[1] );
		cube->SetZLength( request.lengths[2] );

		SetActorProperties( actor, request );
		
	}

	void RenderRequestVisitor::operator()( const LineRenderRequest& request ) {

		vtkLineSource* line;
		vtkSmartPointer<vtkActor> actor;
		
		vtkSmartPointer<vtkLineSource> li = vtkSmartPointer<vtkLineSource>::New();
		vtkSmartPointer<vtkPolyDataMapper> pdm = vtkSmartPointer<vtkPolyDataMapper>::New();
		actor = vtkSmartPointer<vtkActor>::New();

		pdm->SetInputConnection( li->GetOutputPort() );
		actor->SetMapper( pdm );
		manager.renderer->AddActor( actor );

		RendererManager::RenderRegistration registration;
		registration.algorithms.push_back( li );
		registration.mapper = pdm;
		registration.actor = actor;
		manager.registry[ request.id ] = registration;

		line = li.GetPointer();

		line->SetPoint1( request.start[0], request.start[1], request.start[2] );
		line->SetPoint2( request.finish[0], request.finish[1], request.finish[2] );

		SetActorProperties( actor, request );
		
	}

	void RenderRequestVisitor::operator()( const ArrowRenderRequest& request ) {

		// Create arrow
		vtkSmartPointer<vtkArrowSource> arrow = vtkSmartPointer<vtkArrowSource>::New();
		
		// Compute a basis
		double normalizedX[3];
		double normalizedY[3];
		double normalizedZ[3];
		
		// The X axis is a vector from start to end
		vtkMath::Subtract(request.finish, request.start, normalizedX);
		double length = vtkMath::Norm(normalizedX);
		vtkMath::Normalize(normalizedX);
		
		// The Z axis is an arbitrary vector cross X
		double arbitrary[3];
		arbitrary[0] = vtkMath::Random(-1,1);
		arbitrary[1] = vtkMath::Random(-1,1);
		arbitrary[2] = vtkMath::Random(-1,1);
		vtkMath::Cross(normalizedX, arbitrary, normalizedZ);
		vtkMath::Normalize(normalizedZ);
		
		// The Y axis is Z cross X
		vtkMath::Cross(normalizedZ, normalizedX, normalizedY);
		vtkSmartPointer<vtkMatrix4x4> matrix =
			vtkSmartPointer<vtkMatrix4x4>::New();
		
		// Create the direction cosine matrix
		matrix->Identity();
		for (unsigned int i = 0; i < 3; i++)
		{
			matrix->SetElement(i, 0, normalizedX[i]);
			matrix->SetElement(i, 1, normalizedY[i]);
			matrix->SetElement(i, 2, normalizedZ[i]);
		}
		
		// Apply the transforms
		vtkSmartPointer<vtkTransform> transform =
			vtkSmartPointer<vtkTransform>::New();
		transform->Translate(request.start);
		transform->Concatenate(matrix);
		transform->Scale(length, length, length);
		
		// Transform the polydata
		vtkSmartPointer<vtkTransformPolyDataFilter> transformPD =
			vtkSmartPointer<vtkTransformPolyDataFilter>::New();
		transformPD->SetTransform(transform);
		transformPD->SetInputConnection(arrow->GetOutputPort());

		vtkSmartPointer<vtkPolyDataMapper> pdm = vtkSmartPointer<vtkPolyDataMapper>::New();
		pdm->SetInputConnection( transformPD->GetOutputPort() );

		vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
		actor->SetMapper( pdm );
		manager.renderer->AddActor( actor );
		
		RendererManager::RenderRegistration registration;
		registration.algorithms.push_back(arrow);
		registration.algorithms.push_back(transformPD);
		registration.mapper = pdm;
		registration.actor = actor;
		manager.registry[ request.id ] = registration;
		
		// Set arrow properties
		arrow->SetTipLength( request.tipLength );
		arrow->SetTipRadius( request.tipRadius );
		arrow->SetTipResolution( request.tipResolution );
		arrow->SetShaftRadius( request.shaftRadius );
		arrow->SetShaftResolution( request.shaftResolution );
		
		SetActorProperties( actor, request );

	}
	
	void RenderRequestVisitor::operator()( const PlaneRenderRequest& request ) {

		vtkPlaneSource* plane;
		vtkSmartPointer<vtkActor> actor;
		
		vtkSmartPointer<vtkPlaneSource> pl = vtkSmartPointer<vtkPlaneSource>::New();
		vtkSmartPointer<vtkPolyDataMapper> pdm = vtkSmartPointer<vtkPolyDataMapper>::New();
		actor = vtkSmartPointer<vtkActor>::New();

		pdm->SetInputConnection( pl->GetOutputPort() );
		actor->SetMapper( pdm );
		manager.renderer->AddActor( actor );

		RendererManager::RenderRegistration registration;
		registration.algorithms.push_back( pl );
		registration.mapper = pdm;
		registration.actor = actor;
		manager.registry[ request.id ] = registration;

		plane = pl.GetPointer();
		
		plane->SetOrigin( request.center[0], request.center[1], request.center[2] );
		plane->SetPoint1( request.center[0] + request.lengths[0], request.center[1],
						  request.center[2] );
		plane->SetPoint2( request.center[0], request.center[1] + request.lengths[1],
						  request.center[2] );

		plane->SetCenter( request.center[0], request.center[1], request.center[2] );
		plane->SetNormal( request.normal[0], request.normal[1], request.normal[2] );
		
		SetActorProperties( actor, request );
		
	}

	void RenderRequestVisitor::operator()( const ClearRequest& request ) {
		BOOST_FOREACH( const RendererManager::RegistrationMap::value_type& item,
					   manager.registry ) {
			manager.renderer->RemoveActor( item.second.actor );
		}
		manager.registry.clear();

		// TODO Check clearAll flag instead of always clearing all
	}

	void RenderRequestVisitor::SetActorProperties( vtkSmartPointer<vtkActor>& actor,
												   const RenderRequest& request ) {

		vtkProperty* property = actor->GetProperty();

		property->SetLighting( request.useLighting );

		switch( request.shader ) {
			case RENDER_SHADER_FLAT:
				property->SetInterpolationToFlat();
				break;
			case RENDER_SHADER_GOURAUD:
				property->SetInterpolationToGouraud();
				break;
			case RENDER_SHADER_PHONG:
				property->SetInterpolationToPhong();
				break;
			default:
				throw std::runtime_error( "Invalid render shader type." );
		}

		switch( request.representation ) {
			case RENDER_GEOM_POINTS:
				property->SetRepresentationToPoints();
				break;
			case RENDER_GEOM_WIREFRAME:
				property->SetRepresentationToWireframe();
				break;
			case RENDER_GEOM_SURFACE:
				property->SetRepresentationToSurface();
				break;
		}

		property->SetColor( request.color.red, request.color.green, request.color.blue );
		property->SetOpacity( request.opacity );
	}

	InteractorTimerCallback* InteractorTimerCallback::New() {
		InteractorTimerCallback *cb = new InteractorTimerCallback;
		return cb;
	}

	void InteractorTimerCallback::SetCallback( boost::function<void()> _callback ) {
		callback = _callback;
	}

	void InteractorTimerCallback::Execute( vtkObject* caller, unsigned long eventID, void* callData ) {
		if( vtkCommand::TimerEvent != eventID ) { return; }
		
		callback();
	}

	KeyPressInteractorStyle* KeyPressInteractorStyle::New() {
		return new KeyPressInteractorStyle;
	}
	
	void KeyPressInteractorStyle::OnKeyPress() {
	
			// Get the keypress
			vtkRenderWindowInteractor *rwi = this->Interactor;
			std::string key = rwi->GetKeySym();
			
			// TODO Call a more general key checking callback instead
			if(key == "s") {
				callback();
			}
			
			// Forward events
			vtkInteractorStyleTrackballCamera::OnKeyPress();
	}

	void KeyPressInteractorStyle::SetCallback( boost::function<void()> _callback ) {
		callback = _callback;
	}
		
	RendererManager::RendererManager( const std::string& _name, unsigned int xSize,
									  unsigned int ySize ) :
		name( _name ),
		renderer( vtkRenderer::New() ),
		renderWindow( vtkRenderWindow::New() ),
		renderInteractor( vtkXRenderWindowInteractor::New() ),
		windowToImage( vtkWindowToImageFilter::New() ),
		pngWriter( vtkPNGWriter::New() ),
		keypressStyle( vtkSmartPointer<KeyPressInteractorStyle>::New() ),
		screenshotCounter( 0 ),
		screenshotPrefix( "Screenshot" ) {

		XInitThreads();
		XtToolkitInitialize(); // TODO Do only once?
		xAppContext = XtCreateApplicationContext();
			
		renderWindow->AddRenderer( renderer );
		renderWindow->SetWindowName( name.c_str() );
		renderWindow->SetSize( xSize, ySize );

		renderer->SetBackground( 0.2, 0.2, 0.2 );
		
		renderInteractor->SetRenderWindow( renderWindow );
		renderInteractor->Initialize( xAppContext );
		vtkSmartPointer<InteractorTimerCallback> cb =
			vtkSmartPointer<InteractorTimerCallback>::New();
		cb->SetCallback( boost::bind( &RendererManager::ProcessRequestQueue, this ) );
		renderInteractor->AddObserver( vtkCommand::TimerEvent, cb );
		renderInteractor->CreateRepeatingTimer( 100 );
		renderInteractor->SetInteractorStyle( keypressStyle );
		
		keypressStyle->SetCurrentRenderer(renderer);
		keypressStyle->SetCallback( boost::bind( &RendererManager::CaptureScreenshot, this ) );
		
		// Screenshot setup
		windowToImage->SetInput(renderWindow);
		windowToImage->SetInputBufferTypeToRGBA(); //also record the alpha (transparency) channel
		windowToImage->ReadFrontBufferOff(); // read from the back buffer
		SetScreenshotMagnification( 1 );
		
		pngWriter->SetInputConnection(windowToImage->GetOutputPort());

		// Initial render update
		renderWindow->Render();

 		interactorThread =
 			boost::thread( boost::bind( &RendererManager::InteractorLoop, this ) );
	}

	void RendererManager::QueueRenderRequest( RenderRequestVariant& request ) {

		boost::unique_lock<boost::mutex> lock( queueMutex );
		requestQueue.push_back( request );

	}

	void RendererManager::QueueRenderRequests( const std::vector<RenderRequestVariant>& requests ) {
		boost::unique_lock<boost::mutex> lock( queueMutex );
		BOOST_FOREACH( const RenderRequestVariant& req, requests ) {
			requestQueue.push_back( req );
		}
	}

	void RendererManager::ProcessRequestQueue() {

		boost::unique_lock<boost::mutex> lock( queueMutex );
		if( requestQueue.empty() ) {
			return;
		}
		
		RenderRequestVisitor visitor( *this );

		BOOST_FOREACH( RenderRequestVariant& variant, requestQueue ) {
			boost::apply_visitor( visitor, variant );
		}
		requestQueue.clear();
		
		renderWindow->Render();
	}

	void RendererManager::Clear() {
		ClearRequest request;
		RenderRequestVariant vreq = request;
		QueueRenderRequest( vreq );
	}

	void RendererManager::SetScreenshotPrefix( const std::string& _prefix ) {
		screenshotPrefix = _prefix;
	}
	
	void RendererManager::SetScreenshotMagnification( unsigned int mag ) {
		windowToImage->SetMagnification( mag );
	}
	
	void RendererManager::InteractorLoop() {
		XtAppMainLoop( xAppContext );
	}

	void RendererManager::CaptureScreenshot() {

		windowToImage->Modified();
		windowToImage->Update();

		std::stringstream ss;
		ss << screenshotPrefix << screenshotCounter << ".png";
		screenshotCounter++;

		std::cout << "Screenshot saved to " << ss.str() << std::endl;
		
		pngWriter->SetFileName( ss.str().c_str() );
		pngWriter->Write();
	}
	
}