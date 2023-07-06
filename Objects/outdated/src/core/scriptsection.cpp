
#include "core/scriptsection.h"

#include "new.h"

using namespace obj;

ScriptSection* gScriptSection = NULL;

struct script_data_head {
	tp::alni refc = 0;
	tp::alni store_adress = 0;
};

void set_script_head_store_adress(Script* in, tp::alni address) {
	((script_data_head*)in - 1)->store_adress = address;
}

tp::alni get_script_head_store_adress(Script* in) {
	return ((script_data_head*)in - 1)->store_adress;
}

Script* ScriptSection::createScript() {
	auto sdhead = (script_data_head*)tp::pickalloc->Alloc(sizeof(script_data_head) + sizeof(Script));
	auto out = (Script*)(sdhead + 1);

	if (!sdhead) {
		return NULL;
	}

	sdhead->refc = 1;
	sdhead->store_adress = -1;

	mScripts.pushBack(out);

	new (out) Script();
	out->mReadable = obj::StringObject::create("");
	obj::NDO->refinc(out->mReadable);
	return out;
}

void ScriptSection::delete_script(Script* script) {
	script->~Script();
	obj::NDO->destroy(script->mReadable);

	tp::pickalloc->Free((((script_data_head*)script) - 1));
}

void ScriptSection::reference_script(Script* script) {
	(((script_data_head*)script) - 1)->refc++;
}

void ScriptSection::abandonScript(Script* script) {
	if ((((script_data_head*)script) - 1)->refc > 1) {
		(((script_data_head*)script) - 1)->refc--;
	}
	else {
		// ISSUE : on delete list structure is outdated
		// FIXME : use pool alloc with ref count
		// forwarding to global destruction
		//delete_script(script);
	}
}

void ScriptSection::changeScript(Script** current_script, Script** new_script) {
	abandonScript(*current_script);
	reference_script(*new_script);

	*current_script = *new_script;
}

tp::alni ScriptSection::get_script_file_adress(Script* in) {
	return  get_script_head_store_adress(in);
}

tp::alni ScriptSection::save_script_table_to_file_size(ScriptSection* self, tp::File& file) {
	tp::alni size = 0;
	size += 5; // header
	size += sizeof(tp::alni); // scripts length

	for (auto iter : self->mScripts) {

		set_script_head_store_adress(iter.data(), file.adress + size);

		size += sizeof(tp::alni); // scripts string obj ref
		size += sizeof(tp::alni); // constants length

		for (auto const_obj : iter->mBytecode.mConstants) {
			size += sizeof(tp::alni); // constant object addres
		}

		// instructions
		size += iter->mBytecode.mInstructions.saveSize();
	}

	size += sizeof(tp::alni); // objects mem offset
	size += 5; // header
	return size;
}

void ScriptSection::save_script_table_to_file(ScriptSection* self, tp::File& file) {
	
	// header
	file.write_bytes("/scr/", 5);

	// scripts length
	tp::alni scripts_count = self->mScripts.length(); 
	file.write<tp::alni>(&scripts_count);

	for (auto iter : self->mScripts) {
		
		// scripts string obj ref
		auto obj_addres = obj::NDO->save(file, iter->mReadable);
		file.write(&obj_addres);

		// constants length
		tp::alni consts_count = iter->mBytecode.mConstants.length();
		file.write<tp::alni>(&consts_count);

		for (auto const_obj : iter->mBytecode.mConstants) {
			// constant object addres
			auto obj_addres = obj::NDO->save(file, const_obj.data());
			file.write(&obj_addres);
		}

		// mInstructions
		iter->mBytecode.mInstructions.save(file);
	}

	// header
	file.write_bytes("/scr/", 5);

	tp::alni objects_mem_offset = file.avl_adress;
	file.write<tp::alni>(&objects_mem_offset);
}

void load_constants(ScriptSection* self, tp::File& file, tp::alni start_addr) {
	auto addres = file.adress;

	file.adress = start_addr;
	file.adress += 5; // header
	
	tp::alni scripts_count;
	file.read<tp::alni>(&scripts_count);

	for (tp::alni i = 0; i < scripts_count; i++) {
		
		auto script = self->get_scritp_from_file_adress(file.adress);

		// script text
		tp::alni str_addr;
		file.read<tp::alni>(&str_addr);
		script->mReadable = NDO_CAST(obj::StringObject, obj::NDO->load(file, str_addr));

		file.adress += sizeof(tp::alni); // constants length

		for (auto const_obj : script->mBytecode.mConstants) {
			tp::alni consts_addr;
			file.read<tp::alni>(&consts_addr);
			const_obj.data() = obj::NDO->load(file, consts_addr);
		}

		// instructions
		file.adress += script->mBytecode.mInstructions.saveSize();
	}

	file.adress = addres;
}

void ScriptSection::load_script_table_from_file(ScriptSection* self, tp::File& file) {

	for (auto iter : self->mScripts) {
		set_script_head_store_adress(iter.data(), -1);
	}

	auto section_start_addr = file.adress;

	// header
	file.adress += 5;

	// scripts length
	tp::alni scripts_count;
	file.read<tp::alni>(&scripts_count);

	for (tp::alni i = 0; i < scripts_count; i++) {

		auto new_script = self->createScript();
		set_script_head_store_adress(new_script, file.adress);

		// scripts text
		file.adress += sizeof(tp::alni);

		// constants length
		tp::alni consts_count;
		file.read<tp::alni>(&consts_count);

		new_script->mBytecode.mConstants.reserve(consts_count);

		for (tp::alni j = 0; j < consts_count; j++) {
			// constant object addres
			tp::alni consts_addr;
			file.read<tp::alni>(&consts_addr);

			// skiping
			//new_script->mBytecode.mConstants[j] = obj::NDO->load(file, consts_addr);
		}

		// mInstructions
		new_script->mBytecode.mInstructions.load(file);
	}

	load_constants(self, file, section_start_addr);

	// header
	file.adress += 5;

	tp::alni objects_mem_offset;
	file.read<tp::alni>(&objects_mem_offset);

	file.adress = objects_mem_offset;
}

Script* ScriptSection::get_scritp_from_file_adress(tp::alni file_adress) {
	for (auto iter : mScripts) {
		if (get_script_head_store_adress(iter.data()) == file_adress) {
			return iter.data();
		}
	}
	return NULL;
}

ScriptSection::~ScriptSection() {
	for (auto iter : mScripts) {
		delete_script(iter.data());
	}
}

obj::save_load_callbacks ScriptSection::slcb_script_section = {
	.self = gScriptSection,
	.pre_save = (obj::pre_save_callback*)ScriptSection::save_script_table_to_file,
	.size = (obj::slcb_size_callback*)ScriptSection::save_script_table_to_file_size,
	.pre_load = (obj::pre_load_callback*)ScriptSection::load_script_table_from_file,
	.post_save = NULL,
	.post_load = NULL,
};


void ScriptSection::initialize() {
	RelAssert(!gScriptSection);
	gScriptSection = new ScriptSection();

	RelAssert(obj::NDO && "Forgot to initialize objects?");

	slcb_script_section.self = gScriptSection;
	obj::NDO->add_sl_callbacks(&slcb_script_section);
}

void ScriptSection::uninitialize() {
	RelAssert(gScriptSection);
	delete gScriptSection;
}

ScriptSection* ScriptSection::globalHandle() {
	return gScriptSection;
}

