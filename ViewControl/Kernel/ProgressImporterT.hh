#pragma once

#include "../ViewCtrl.h"

#include <OpenMesh/Core/IO/importer/ImporterT.hh>
#include <OpenMesh/Core/IO/MeshIO.hh>

static int g_CurFace = 1;

// importer
namespace OpenMesh {
	namespace IO {


		//=== IMPLEMENTATION ==========================================================


		/**
		 *  This class template provides an importer module for OpenMesh meshes.
		 */
		template <class Mesh>
		class ProgressImporterT : public ImporterT<Mesh>
		{
		public:
			using ImporterT::ImporterT;

			FaceHandle add_face(const VHandles& _vhandles) override
			{
				MSG  msg;
				if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				GetGlobalView()->setLoadingFacesCur(g_CurFace);
				g_CurFace++;
				return ImporterT::add_face(_vhandles);

			}
		};
	}
}

// mesh IO
namespace OpenMesh {
	namespace IO {

		template <class Mesh>
		bool
			read_mesh_progress(Mesh&         _mesh,
				const std::string&  _filename)
		{
			Options opt;
			return read_mesh_progress(_mesh, _filename, opt, true);
		}

		template <class Mesh>
		bool
			read_mesh_progress(Mesh&         _mesh,
				const std::string&  _filename,
				Options&            _opt,
				bool                _clear = true)
		{
			if(_clear) _mesh.clear();
			ProgressImporterT<Mesh> importer(_mesh);
			return IOManager().read(_filename, importer, _opt);
		}
	}
}

