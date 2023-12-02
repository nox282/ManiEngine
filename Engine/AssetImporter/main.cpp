#include <Core/Log.h>
#include <Core/ConfigSystem.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Shader.h>

#include <MeshImporter/MeshImporter.h>
#include <ShaderImporter/ShaderImporter.h>

#include <vector>
#include <memory>

using namespace ECSEngine;
namespace fs = std::filesystem;

struct AssetImporterConfig : IBaseConfig
{
	std::string engineAssetPath = "Engine/Assets";
	std::string projectAssetPath = "";
};

void from_json(const nlohmann::json& json, AssetImporterConfig& object)
{
	json.at("engineAssetPath").get_to(object.engineAssetPath);
	json.at("projectAssetPath").get_to(object.projectAssetPath);
}

void exploreDirectory(const fs::path& path);
void importModel(const fs::path& path);
void importShader(const fs::path& path);

int main(int argc, char** argv)
{
	Application app;
	std::shared_ptr<ConfigSystem> configSystem = app.getSystemContainer().initializeDependency<ConfigSystem>().lock();
	std::shared_ptr<AssetImporterConfig> config = configSystem->getConfig<AssetImporterConfig>("AssetImporter");
	fs::path rootPath;
	if (!FileSystem::tryGetRootPath(rootPath))
	{
		ECSE_LOG_ERROR(Log, "Could not get root path");
		return EXIT_FAILURE;
	}

	if (!config->engineAssetPath.empty())
	{
		fs::path engineAssetPath = fs::path(rootPath).append(config->engineAssetPath);
		exploreDirectory(engineAssetPath);
	}

	if (!config->projectAssetPath.empty())
	{
		fs::path projectAssetPath = fs::path(rootPath).append(config->projectAssetPath);
		exploreDirectory(projectAssetPath);
	}

	return EXIT_SUCCESS;
}

void exploreDirectory(const fs::path& path)
{
	for (const auto& entry : fs::recursive_directory_iterator(path))
	{
		if (entry.is_directory() || !entry.is_regular_file())
		{
			continue;
		}

		fs::path extension = entry.path().extension();

		if (extension == ".glsl")
		{
			ECSE_LOG(Log, "importing {}", entry.path().string());
			importShader(entry.path());
		}
		else if (extension == ".fbx")
		{
			ECSE_LOG(Log, "importing {}", entry.path().string());
			importModel(entry.path());
		}
	}
}

void importModel(const fs::path& path)
{
	std::vector<std::shared_ptr<Mesh>> meshes;
	if (!MeshImporter::importFromPath(path, meshes))
	{
		ECSE_LOG_ERROR(Log, "Could not import mesh at {}", path.string());
		return;
	}

	for (const std::shared_ptr<Mesh>& mesh : meshes)
	{
		fs::path output = path.parent_path();
		output.append(std::format("{}{}", mesh->name, ".mesh"));
		ECSE_LOG(Log, "Saving {}", output.string());
		if (!MeshImporter::exportToPath(output, mesh))
		{
			ECSE_LOG_ERROR(Log, "Could not save mesh at {}", output.string());
		}
	}
}

void importShader(const fs::path& path)
{
	std::shared_ptr<Shader> shader = std::make_shared<Shader>();
	if (!ShaderImporter::importFromPath(path, shader))
	{
		ECSE_LOG_ERROR(Log, "Could not import shader at {}", path.string());
		return;
	}

	fs::path output = path.parent_path();
	output.append(std::format("{}{}", shader->name, ".shader"));
	ECSE_LOG(Log, "Saving {}", output.string());
	if (!ShaderImporter::exportToPath(output, shader))
	{
		ECSE_LOG_ERROR(Log, "Could not save shader at {}", output.string());
	}
}
