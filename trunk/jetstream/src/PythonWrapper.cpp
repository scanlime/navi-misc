/* 
 * PythonWrapper.cpp - Interface for the JetEngine python module using Boost
 *
 * Copyright (C) 2002-2004 Micah Dowty and David Trowbridge
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <boost/python.hpp>
#include <boost/python/reference_existing_object.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/python/bases.hpp>
using namespace boost::python;
using boost::python::return_value_policy;
using boost::python::reference_existing_object;
using boost::python::bases;

#include "Engine.h"
#include "EmbeddedPGserver.h"
#include "Scene.h"
#include "Surface.h"
#include "VideoRecorder.h"
#include "Skybox.h"
#include "Camera.h"
#include "AnimatedVector.h"
#include "Vector.h"
#include "ParticleSystem.h"
#include "DetailSettings.h"
#include "Spline.h"
#include "SplineSceneNode.h"

BOOST_PYTHON_MODULE(JetEngine)
{

  class_<DetailSettings, boost::noncopyable>("DetailSettings")
    .def_readwrite("geomorphDuration", &DetailSettings::geomorphDuration)
    .def_readwrite("minTesselationErrorSquared", &DetailSettings::minTesselationErrorSquared)
    .def_readwrite("maxTesselationErrorSquared", &DetailSettings::maxTesselationErrorSquared)
    ;

  class_<Engine>("Engine")
    .def("iteration", &Engine::iteration)
    .def_readwrite("frustumLock", &Engine::frustumLock)
    .def_readwrite("running", &Engine::running)
    .def("getDetail", &Engine::getDetail,
	 return_value_policy<reference_existing_object>())
    ;

  class_<EmbeddedPGserver>("EmbeddedPGserver")
    .def("getParam", &EmbeddedPGserver::getParamInt)
    .def("getParam", &EmbeddedPGserver::getParamStr)
    .def("setParam", &EmbeddedPGserver::setParam)
    .def("init", &EmbeddedPGserver::init)
    .def("shutdown", &EmbeddedPGserver::shutdown)
    .def("write", &EmbeddedPGserver::write)
    .def("read", &EmbeddedPGserver::read)
    .def("getShowFPS", &EmbeddedPGserver::getShowFPS)
    .def("setShowFPS", &EmbeddedPGserver::setShowFPS)
    ;
  
  class_<VideoRecorder>("VideoRecorder")
    .def("start", &VideoRecorder::start)
    .def("stop", &VideoRecorder::stop)
    .def("isRunning", &VideoRecorder::isRunning)
    .def_readwrite("width", &VideoRecorder::width)
    .def_readwrite("height", &VideoRecorder::height)
    .def_readwrite("framerate", &VideoRecorder::framerate)
    .def_readwrite("filename", &VideoRecorder::filename)
    .def_readwrite("quality", &VideoRecorder::quality)
    .def_readwrite("rtjpeg", &VideoRecorder::rtjpeg)
    .def_readwrite("lzo", &VideoRecorder::lzo)
    .def_readwrite("rgb", &VideoRecorder::rgb)
    ;


  class_<JetCOWObject, boost::noncopyable>("JetCOWObject", no_init)
    .def("ref", &JetCOWObject::ref)
    .def("unref", &JetCOWObject::unref)
    .def_readonly("id", &JetCOWObject::id)
    .def_readonly("type", &JetCOWObject::type)
    ;

  class_<JetCOWDictionary, bases<JetCOWObject>, boost::noncopyable>("JetCOWDictionary", no_init)
    .def("setAttrInt", &JetCOWDictionary::setAttrInt)
    .def("setAttrStr", &JetCOWDictionary::setAttrStr)
    .def("setAttrFloat", &JetCOWDictionary::setAttrFloat)
    .def("getAttrInt", &JetCOWDictionary::getAttrInt)
    .def("getAttrStr", &JetCOWDictionary::getAttrSTLStr)
    .def("getAttrFloat", &JetCOWDictionary::getAttrFloat)
    .def("delAttr", &JetCOWDictionary::delAttr)
    .def("revert", &JetCOWDictionary::revert)
    .def("commit", &JetCOWDictionary::commit)
    ;

  class_<SceneNode, bases<JetCOWDictionary>, boost::noncopyable>("SceneNode", no_init)
    .def("attachChild", &SceneNode::attachChild)
    .def("removeChild", &SceneNode::removeChild)
    ;

  class_<Scene, bases<SceneNode>, boost::noncopyable >("Scene", no_init)
    .def_readwrite("wireframe", &Scene::wireframe)
    .def_readwrite("disableBackfaceCull", &Scene::disableBackfaceCull)
    .def_readwrite("bgShade", &Scene::bgShade)
    .def("getCamera", &Scene::getCamera,
	 return_value_policy<reference_existing_object>())
    ;

  class_<Skybox, bases<SceneNode>, boost::noncopyable >("Skybox", no_init);

  class_<Surface, bases<SceneNode>, boost::noncopyable >("Surface", no_init);

  class_<ParticleSystem, bases<SceneNode>, boost::noncopyable >("ParticleSystem", no_init);

  class_<SplineSceneNode, bases<SceneNode> >("SplineSceneNode", no_init)
    .def("set1", &SplineSceneNode::set1)
    .def("get1", &SplineSceneNode::get1)
    .def("setv1", &SplineSceneNode::setv1)
    ;

  class_<JetCOW>("JetCOW", no_init)
    .def(init<const char *>())
    .def(init<const char *,bool>())
    .def(init<const char *,bool,bool>())
    .def(init<const char *,bool,bool,Uint32>())
    .def(init<const char *,bool,bool,Uint32,Uint32>())
    .def("flush", &JetCOW::flush)
    .def("fork", &JetCOW::fork,
	 return_value_policy<reference_existing_object>())
    .def("getHeader", &JetCOW::getHeader,
	 return_value_policy<reference_existing_object>())
    .def("checkout", &JetCOW::checkout,
	 return_value_policy<reference_existing_object>())
    .def("checkoutDict", &JetCOW::checkoutDict,
	 return_value_policy<reference_existing_object>())
    .def("newDict", &JetCOW::newDict,
	 return_value_policy<reference_existing_object>())
    .def("checkoutSceneNode", &JetCOW::checkoutSceneNode,
	 return_value_policy<reference_existing_object>())
    .def("newSpline", &JetCOW::newSpline,
         return_value_policy<reference_existing_object>())
    .def("newSceneNode", &JetCOW::newSceneNode,
	 return_value_policy<reference_existing_object>())
    .def("checkoutScene", &JetCOW::checkoutScene,
	 return_value_policy<reference_existing_object>())
    .def("newScene", &JetCOW::newScene,
	 return_value_policy<reference_existing_object>())
    ;
  
  class_<Vector4>("Vector4")
    .def(init<float,float,float,float>())
    .def("set", &Vector4::set)
    .def("set", &Vector4::set1)
    .def("set", &Vector4::setv)
    .def("get", &Vector4::get1)
    .def("length", &Vector4::length)
    .def("length2", &Vector4::length2)
    .def(self + self)
    .def(self - self)
    .def(self * self)
    .def(self * float())
    .def(self += self)
    .def(self -= self)
    .def(self *= float())
    .def(self /= float())
    ;

  class_<Vector3>("Vector3")
    .def(init<float,float,float>())
    .def("set", &Vector3::set)
    .def("set", &Vector3::set1)
    .def("set", &Vector3::setv)
    .def("get", &Vector3::get1)
    .def("length", &Vector3::length)
    .def("length2", &Vector3::length2)
    .def(self + self)
    .def(self - self)
    .def(self * self)
    .def(self % self)
    .def(self * float())
    .def(self += self)
    .def(self -= self)
    .def(self %= self)
    .def(self *= float())
    .def(self /= float())
    ;

  class_<Vector2>("Vector2")
    .def(init<float,float>())
    .def("set", &Vector2::set)
    .def("set", &Vector2::set1)
    .def("set", &Vector2::setv)
    .def("get", &Vector2::get1)
    .def("length", &Vector2::length)
    .def("length2", &Vector2::length2)
    .def(self + self)
    .def(self * self)
    .def(self * float())
    .def(self += self)
    .def(self -= self)
    .def(self *= float())
    .def(self /= float())
    ;

  class_<AnimatedVector, bases<Vector3> >("AnimatedVector")
    /* These inspector functions with the reference_existing_object
     * policy are necessary so the variable is used as-is instead
     * of being copied. Normally the copying is there as a safeguard,
     * but we need this since the values will be modified.
     * We just have to trust the python code won't use
     * the returned values after this class is destroyed.
     */
    .def("getVelocity", &AnimatedVector::getVelocity,
	 return_value_policy<reference_existing_object>())
    .def("getAcceleration", &AnimatedVector::getAcceleration,
	 return_value_policy<reference_existing_object>())
    .def("getTarget", &AnimatedVector::getTarget,
	 return_value_policy<reference_existing_object>())
    .def_readwrite("thrust", &AnimatedVector::thrust)
    .def_readwrite("friction", &AnimatedVector::friction)
    .def_readwrite("hysteresis", &AnimatedVector::hysteresis)
    .def("set", &AnimatedVector::set)
    ;

  class_<Camera>("Camera")
    .def("getPosition", &Camera::getPosition,
	 return_value_policy<reference_existing_object>())
    .def("getRotation", &Camera::getRotation,
	 return_value_policy<reference_existing_object>())
    .def("getDisplacement", &Camera::getDisplacement,
	 return_value_policy<reference_existing_object>())
    .def("getForwardVector", &Camera::getForwardVector)
    .def("getUpVector", &Camera::getUpVector)
    .def_readwrite("forwardThrust", &Camera::forwardThrust)
    ;
}


/* The End */
