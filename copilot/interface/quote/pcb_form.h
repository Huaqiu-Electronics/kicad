/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2025 Ethan Chien <liangtie.qian@gmail.com>
 * Copyright (C) 2025 KiCad Developers, see AUTHORS.txt for contributors.
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
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#ifndef PCB_FORM_H
#define PCB_FORM_H

#include <nlohmann/json.hpp>

// BaseInfoModel
struct PCB_FORM
{
    std::string blayer = "4";         // Layer Count
    std::string blength = "31.22";    // GetPcbLength
    std::string bwidth = "10.69";     // GetPcbWidth
    std::string bcount = "5";         // Qty(single)
    std::string sidedirection = "无"; // Decided by the marginMode automatically

    std::string plate_type = "FR-4"; // Material Type
    std::string units = "1";         // Board Type
    std::string layoutx = "0";       // X
    std::string layouty = "0";       // Y
    std::string sidewidth = "0";     // Break-away Rail

    int         testpoint = 0; // 测试点数，默认为0
    int         pbnum = 1;     // 拼版款数，指文件内不同款的板子个数， 不传默认为1
    std::string board_tg;      // 4层及以上可选TG值，TG130、TG150、TG170

    // ProcessInfoModel
    std::string bheight = "2.0";      // PCB Thickness
    std::string copper = "1";         // Finished Copper Weight
    std::string lineweight = "4";     // Min Trace/Space Outer
    std::string vias = "0.3";         // Min Drilled Hole
    std::string color = "绿色";       // Solder Mask Color
    std::string charcolor = "白色";   // Silkscreen
    std::string cover = "过孔盖油";   // Via Process
    std::string spray = "有铅喷锡";   // Surface Finish
    std::string insidecopper = "0.5"; // Inner Copper Weight

    // SpecialProcessModel
    std::string impendance = "0";  // Impendance
    std::string bankong = "0";     // Plated Half Holes:
    std::string blind = "0";       // HDI(Buried/blind vias):
    std::string via_in_pad = "无"; // Pad Hole:
    std::string beveledge = "0";   // Beveling of G/F:
    std::string pressing = "";     // Stack up

    std::string baobian = "0"; // 包边，默认 0 无 1-4 边
    std::string bga;           // BGA，默认0 无，可选值 0.35 – 0.35及以上 0.25  #CN only
    std::string zknum;         // 钻孔密度

    // PersonalizedInfoModel
    std::string test = "Sample Test Free"; // Electrical Test
    std::string shipment_report = "0";     // Delivery Report
    std::string slice_report = "0";        // Microsection Analysis Report
    std::string report_type = "0";         // Report Format
    std::string review_file = "0"; // Approve Working Gerber:  确认生产稿 0无需 1需要-自动确认 2需要-非自动确认
    std::string has_period = "2";  // Decided by period_format
    std::string period_format;     // UL Mark
    std::string film_report = "0"; // Film
    std::string pcb_note = "";     // Special Requests # Non-CN

    int cross_board = 1; // 打叉板 1接受 2不接受
    int paper = 1;       // 隔白纸 1无需 2需要
    int user_stamp = 1;  // 不加客编 1无要求 2指定位置加客编 3不加客编
    int hq_pack = 1;     // 包装要求，默认 1华秋包装，0中性包装  #CN only

    // Added fields from default form that weren't in the original struct:
    std::string service;   // Service type
    std::string region_id; // Region ID
    std::string country;   // Country
    std::string express;   // Express option
    std::string type;      // File type

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( PCB_FORM, blayer, blength, bwidth, bcount, sidedirection, plate_type, units,
                                    layoutx, layouty, sidewidth, testpoint, pbnum, board_tg, bheight, copper,
                                    lineweight, vias, color, charcolor, cover, spray, insidecopper, impendance, bankong,
                                    blind, via_in_pad, beveledge, pressing, baobian, bga, zknum, test, shipment_report,
                                    slice_report, report_type, review_file, has_period, period_format, film_report,
                                    pcb_note, cross_board, paper, user_stamp, hq_pack, service, region_id, country,
                                    express, type )
};

#endif
