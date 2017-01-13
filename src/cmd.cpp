/*

Osmium -- OpenStreetMap data manipulation command line tool
http://osmcode.org/osmium

Copyright (C) 2013-2017  Jochen Topf <jochen@topf.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <cstdlib>
#include <iostream>

#include <osmium/osm/entity_bits.hpp>
#include <osmium/util/memory.hpp>
#include <osmium/util/verbose_output.hpp>

#include "cmd.hpp"
#include "exception.hpp"

const char* yes_no(bool choice) noexcept {
    return choice ? "yes\n" : "no\n";
}

void warning(const char* text) {
    std::cerr << "WARNING: " << text;
}

po::options_description Command::add_common_options() {
    po::options_description options("COMMON OPTIONS");

    options.add_options()
    ("help,h", "Show usage help")
    ("progress", "Display progress bar")
    ("no-progress", "Suppress display of progress bar")
    ("verbose,v", "Set verbose mode")
    ;

    return options;
}

void Command::setup_common(const boost::program_options::variables_map& vm, const po::options_description& desc) {
    if (vm.count("help")) {
        std::cout << "Usage: " << synopsis() << "\n\n"
                  << CommandFactory::get_description(name()) << "\n"
                  << desc
                  << "\nUse 'osmium help " << name() << "' to display the manual page.\n";
        std::exit(0);
    }

    if (vm.count("verbose")) {
        m_vout.verbose(true);
    }
}

void Command::setup_progress(const boost::program_options::variables_map& vm) {
    if (vm.count("progress") && vm.count("no-progress")) {
        throw argument_error{"Can not use --progress and --no-progress together."};
    }

    if (vm.count("progress")) {
        m_display_progress = display_progress_type::always;
    }

    if (vm.count("no-progress")) {
        m_display_progress = display_progress_type::never;
    }
}

void Command::setup_object_type_nrwc(const boost::program_options::variables_map& vm) {
    if (vm.count("object-type")) {
        m_osm_entity_bits = osmium::osm_entity_bits::nothing;
        for (const auto& t : vm["object-type"].as<std::vector<std::string>>()) {
            if (t == "n" || t == "node") {
                m_osm_entity_bits |= osmium::osm_entity_bits::node;
            } else if (t == "w" || t == "way") {
                m_osm_entity_bits |= osmium::osm_entity_bits::way;
            } else if (t == "r" || t == "relation") {
                m_osm_entity_bits |= osmium::osm_entity_bits::relation;
            } else if (t == "c" || t == "changeset") {
                m_osm_entity_bits |= osmium::osm_entity_bits::changeset;
            } else {
                throw argument_error{std::string{"Unknown object type '"} + t + "' (Allowed are 'node', 'way', 'relation', and 'changeset')."};
            }
        }
    } else {
        m_osm_entity_bits = osmium::osm_entity_bits::all;
    }
}

void Command::setup_object_type_nrw(const boost::program_options::variables_map& vm) {
    if (vm.count("object-type")) {
        m_osm_entity_bits = osmium::osm_entity_bits::nothing;
        for (const auto& t : vm["object-type"].as<std::vector<std::string>>()) {
            if (t == "n" || t == "node") {
                m_osm_entity_bits |= osmium::osm_entity_bits::node;
            } else if (t == "w" || t == "way") {
                m_osm_entity_bits |= osmium::osm_entity_bits::way;
            } else if (t == "r" || t == "relation") {
                m_osm_entity_bits |= osmium::osm_entity_bits::relation;
            } else {
                throw argument_error{std::string{"Unknown object type '"} + t + "' (Allowed are 'node', 'way', and 'relation')."};
            }
        }
    } else {
        m_osm_entity_bits = osmium::osm_entity_bits::nwr;
    }
}

void Command::show_object_types(osmium::util::VerboseOutput& vout) {
    vout << "    object types:";
    if (osm_entity_bits() & osmium::osm_entity_bits::node) {
        vout << " node";
    }
    if (osm_entity_bits() & osmium::osm_entity_bits::way) {
        vout << " way";
    }
    if (osm_entity_bits() & osmium::osm_entity_bits::relation) {
        vout << " relation";
    }
    if (osm_entity_bits() & osmium::osm_entity_bits::changeset) {
        vout << " changeset";
    }
    m_vout << '\n';
}

void Command::print_arguments(const std::string& command) {
    if (m_vout.verbose()) {
        m_vout << "Started osmium " << command << '\n'
               << "  " << get_osmium_long_version() << '\n'
               << "  " << get_libosmium_version() << '\n'
               << "Command line options and default settings:\n";
        show_arguments();
    }
}

void Command::show_memory_used() {
    osmium::MemoryUsage mem;
    if (mem.current() > 0) {
        m_vout << "Peak memory used: " << mem.peak() << " MBytes\n";
    }
}

