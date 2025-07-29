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

#ifndef PCB_ORDER_H
#define PCB_ORDER_H

#include <nlohmann/json.hpp>


static constexpr auto kPCB_QUOTE_API = "https://www.eda.cn/openapi/api/hqpcb/External/fileQuote";

// BaseInfoModel
struct PCB_ORDER
{
    std::string blayer;        // Layer Count
    std::string blength;       // GetPcbLength
    std::string bwidth;        // GetPcbWidth
    std::string bcount;        // Qty(single)
    std::string sidedirection; // Decided by the marginMode automatically

    std::string plate_type = "Fr-4"; // Material Type
    std::string units = "2";         // Board Type
    std::string layoutx;             // X
    std::string layouty;             // Y
    std::string sidewidth;           // Break-away Rail

    int         testpoint = 0; // 测试点数，默认为0
    int         pbnum = 1;     // 拼版款数，指文件内不同款的板子个数， 不传默认为1
    std::string board_tg;      // 4层及以上可选TG值，TG130、TG150、TG170

    // ProcessInfoModel
    std::string bheight;            // PCB Thickness
    std::string copper;             // Finished Copper Weight
    std::string lineweight;         // Min Trace/Space Outer
    std::string vias;               // Min Drilled Hole
    std::string color;              // Solder Mask Color
    std::string charcolor;          // Silkscreen
    std::string cover;              // Via Process
    std::string spray;              // Surface Finish
    std::string insidecopper = "0"; // Inner Copper Weight
    std::string cjh;                // SurfaceProcessCtrl

    // SpecialProcessModel
    std::string impendance;    // Impendance
    std::string bankong;       // Plated Half Holes:
    std::string blind;         // HDI(Buried/blind vias):
    std::string via_in_pad;    // Pad Hole:
    std::string beveledge;     // Beveling of G/F:
    std::string pressing = ""; // Stack up

    std::string baobian; // 包边，默认 0 无 1-4 边
    std::string bga;     // BGA，默认0 无，可选值 0.35 – 0.35及以上 0.25  #CN only
    std::string zknum;   // 钻孔密度

    // PersonalizedInfoModel
    std::string test;            // Electrical Test
    std::string shipment_report; // Delivery Report
    std::string slice_report;    // Microsection Analysis Report
    std::string report_type;     // Report Format
    std::string review_file; // Approve Working Gerber:  确认生产稿 0无需 1需要-自动确认 2需要-非自动确认
    std::string has_period;    // Decided by period_format
    std::string period_format; // UL Mark
    std::string film_report;   // Film
    std::string pcb_note;      // Special Requests # Non-CN

    int cross_board; // 打叉板 1接受 2不接受
    int paper;       // 隔白纸 1无需 2需要
    int user_stamp;  // 不加客编 1无要求 2指定位置加客编 3不加客编
    int hq_pack = 1; // 包装要求，默认 1华秋包装，0中性包装  #CN only

    // File fields , optional , can also attached to the file in the http request
    std::string pcb_file_name;
    std::string pcb_file_path;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( PCB_ORDER, blayer, blength, bwidth, bcount, sidedirection, plate_type, units,
                                    layoutx, layouty, sidewidth, testpoint, pbnum, board_tg, bheight, copper,
                                    lineweight, vias, color, charcolor, cover, spray, insidecopper, cjh, impendance,
                                    bankong, blind, via_in_pad, beveledge, pressing, baobian, bga, zknum, test,
                                    shipment_report, slice_report, report_type, review_file, has_period, period_format,
                                    film_report, pcb_note, cross_board, paper, user_stamp, hq_pack, pcb_file_name,
                                    pcb_file_path )
};

#endif
