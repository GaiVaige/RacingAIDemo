#pragma once
#include "raylib.h"
#include "raymath.h"
#include "vector"
#include <iostream>
struct Collider {
	Collider(Vector2 max, Vector2 ori, float ang) : m_max(max), m_origin(ori), m_rot(ang) {
		m_min = Vector2{ 0,0 };
		m_w = m_max.x;
		m_h = m_max.y;
		float nMinX = INFINITY;
		float nMinY = INFINITY;
		float nMaxX = -INFINITY;
		float nMaxY = -INFINITY;
		m_verts[0] = m_min;
		m_verts[1] = {m_max.x, m_min.y};
		m_verts[2] = m_max;
		m_verts[3] = Vector2{m_min.x, m_max.y};
		GenerateMid();
		for (int i = 0; i < 4; i++) {
			m_verts[i] = Vector2Rotate(m_verts[i], m_rot);
			m_verts[i] = Vector2Add(m_verts[i], m_origin);
		}

		GenerateNormals();
	}
	Vector2 m_min, m_max, m_origin;
	Vector2 m_verts[4];
	Vector2 m_norms[4];
	Vector2 m_midPoint;
	float m_w, m_h;
	float m_rot;
	bool m_overlappedLastFrame;
	bool picked = false;

	void Draw() {
		DrawLine(m_verts[0].x, m_verts[0].y, m_verts[1].x, m_verts[1].y, RED);
		DrawLine(m_verts[1].x, m_verts[1].y, m_verts[2].x, m_verts[2].y, RED);
		DrawLine(m_verts[2].x, m_verts[2].y, m_verts[3].x, m_verts[3].y, RED);
		DrawLine(m_verts[3].x, m_verts[3].y, m_verts[0].x, m_verts[0].y, RED);

	}

	void GenerateMid() {
		float mx = m_max.x;
		float my = m_max.y;
		mx /= 2;
		my /= 2;
		m_midPoint = Vector2{ mx, my };
		m_midPoint = Vector2Rotate(m_midPoint, m_rot);
		m_midPoint = Vector2Add(m_origin, m_midPoint);
	}

	void DrawStart() {
		Rectangle r{ m_origin.x, m_origin.y, 30, 75 };
		DrawRectanglePro(r, Vector2{ 0,0 }, m_rot, GREEN);
	}

	void DrawHighlighted() {
			Rectangle r{ m_origin.x, m_origin.y, 30, 75 };
		if (!picked) {
			DrawRectanglePro(r, Vector2{0,0}, m_rot, SKYBLUE);
		}
		else {
			DrawRectanglePro(r, Vector2{ 0,0 }, m_rot, BLUE);

		}
	}

	void UpdatePosition(Vector2 v, float damp = 1) {
		if (damp == 0) return;
		float mult = Vector2Length(v)/damp;
		v = Vector2Normalize(v);
		if (isnan(v.x) || isnan(v.y)) return;
		v.x *= mult;
		v.y *= mult;
		for (int i = 0; i < 4; i++) {
			m_verts[i] = Vector2Subtract(m_verts[i], m_origin);
			m_verts[i] = Vector2Rotate(m_verts[i], -m_rot);
				m_verts[i] = Vector2Rotate(m_verts[i], m_rot);
				m_verts[i] = Vector2Add(m_verts[i], Vector2Add(m_origin, v));
		}
		m_origin = Vector2Add(m_origin, v);
		GenerateMid();
	}

	void UpdateRotation(float nRot) {
		if (isnan(nRot)) return;
		for (int i = 0; i < 4; i++) {
			m_verts[i] = Vector2Subtract(m_verts[i], m_origin);
			m_verts[i] = Vector2Rotate(m_verts[i], -m_rot);
			m_verts[i] = Vector2Rotate(m_verts[i], m_rot + nRot);
			m_verts[i] = Vector2Add(m_verts[i], m_origin);
		}
		m_rot += nRot;
		GenerateMid();
	}

	bool CheckForOverlap(Vector2 v) {
		v = Vector2Subtract(v, m_origin);
		v = Vector2Rotate(v, -m_rot);
		bool inBoundX = (v.x > m_min.x && v.x < m_max.x);
		bool inBoundY = (v.y > m_min.y && v.y < m_max.y);

		return (inBoundX && inBoundY);
	}

	bool CheckForEntry(Vector2& v) {
		bool isOverlapped = CheckForOverlap(v);
		if (isOverlapped) {
			if (!m_overlappedLastFrame) {
			m_overlappedLastFrame = true;
			return true;
			}
			else {
				return false;
			}
		}
		else {
			m_overlappedLastFrame = false;
			return false;
		}
	}

private:
	void GenerateNormals() {
		for (int i = 0; i < 4; i++) {
			int x = i;
			if (x == 3) {
				x = -1;
			}
			m_norms[i] = GenN(m_verts[i], m_verts[x + 1]);
		}
	}
	Vector2 GenN(Vector2 a, Vector2 b) {
		float dx, dy;
		dx = b.x - a.x;
		dy = b.y - a.y;
		return Vector2Normalize(Vector2{ dy, -dx });
	}


};



class Flag {
public:
	Flag(Vector2 ori, Vector2 cSize, Color col, float rot) : m_pos(ori), m_dCol(col) {
		coll = new Collider(cSize, ori, rot);
	}
	~Flag() {};
	Vector2 m_pos, m_dirToNext;
	size_t flagIndex;
	Color m_dCol;
	Collider* coll;
	Flag* m_nextFlag = nullptr;
	Flag* m_previousFlag = nullptr;
	bool m_hasPassed;

};

class Racer {
public:
	Racer();
	Racer(Flag* startingLine, int lapCount, float ds, float tol, std::vector<Flag*>& fv) : m_driveSpeed(ds), m_tolerance(tol), m_lapCount(lapCount) {
		m_pos = startingLine->coll->m_midPoint;
		points = fv;
		storedFlag = fv[1];
		target = points[1]->coll->m_midPoint;
		nextTarget = points[2]->coll->m_midPoint;
	}
	Vector2 m_pos, m_travelDir;
	Vector2 m_velo = Vector2{ 0,0 };
	Vector2 target, nextTarget;
	float m_tolerance;
	float m_driveSpeed;
	int m_lapCount;
	float m_marchingDist;
	std::vector<Flag*> points;
	Flag* storedFlag;
	int segment;
	bool refreshTarget;

	void Update(float dt = GetFrameTime()) {
		Vector2 add = GenerateTargetPoint();
		add = Vector2Scale(add, (m_driveSpeed * dt));
		m_pos = Vector2Add(m_pos, add);
	}


	Vector2 GenerateTargetPoint() {

		if(refreshTarget){
			target = nextTarget;
			nextTarget = storedFlag->m_nextFlag->m_nextFlag->coll->m_midPoint;
			storedFlag = storedFlag->m_nextFlag;
			refreshTarget = false;
		}

		Vector2 dirToTarg = Vector2Subtract(target, m_pos);
		dirToTarg = Vector2Normalize(dirToTarg);

		Vector2 dirToNextTarg = Vector2Subtract(nextTarget, m_pos);
		dirToNextTarg = Vector2Normalize(dirToNextTarg);
		Vector2 moveDir = dirToTarg;
		moveDir = Vector2Normalize(moveDir);

		float diff = Vector2DotProduct(moveDir, dirToNextTarg);
		if (diff < .1) diff = .1;
		if (diff > 1) diff = 1;
		Vector2 fMoveDir = Vector2Add(Vector2Scale(moveDir, 1 + diff), Vector2Scale(dirToNextTarg, diff));
		fMoveDir = Vector2Normalize(fMoveDir);

		return fMoveDir;

	}



	void Draw() {

		DrawCircle(m_pos.x, m_pos.y, 10, BLACK);

	}


};