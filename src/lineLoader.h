#pragma once
#include "ofMain.h"

class lineLoader {
    public:
    /// Load a JSON file and return a vector of ofPolylines
    static vector<ofPolyline> loadJson(string filename) {
        vector<ofPolyline> lines;
        ofJson json;
        ofFile file(filename);

        if (file.exists()) {
            ofLogNotice("lineLoader::loadJson") << file.getAbsolutePath() << " loaded";
            file >> json;
            for (auto &stroke : json) {
                if (!stroke.empty()) {
                    // path.moveTo(stroke[0]["x"], stroke[0]["y"]);
                    lines.push_back(ofPolyline());
                    for (auto &p : stroke) {
                        lines.back().addVertex(p["x"], p["y"]);
                    }
                }
            }
        } else {
            ofLogNotice("lineLoader::loadJson") << filename << " does not exist";
        }
        return lines;
    };

    /// Save a vector of ofPolylines to a JSON file
    static bool saveJson(string filename, vector<ofPolyline> lines) {

        ofJson json;
        ofJson pt;
        ofJson stroke;

        // Clear the JSON root
        json.clear();

        for (int i = 0; i < lines.size(); i++) {
            for (int j = 0; j < lines[i].size(); j++) {
                pt["x"] = lines[i][j].x;
                pt["y"] = lines[i][j].y;
                stroke.push_back(pt);
            }
            json.push_back(stroke);
            stroke.clear();
        }

        if (ofSaveJson(filename, json)) {
            ofLogNotice("lineLoader::saveJson") << filename << " saved";
            return true;
        } else {
            ofLogNotice("lineLoader::saveJson") << filename << " could not be saved";
            return false;
        }
    };
};
