#include "DrawSnapshot.hpp"
#include "core/graph/SimulationSnapshot.hpp"
#include <raylib.h>

void DrawSnapshot(const SimulationSnapshot& snap) {
    int y = 100;

    DrawText(TextFormat("Tick: %llu", snap.tick), 20, y, 20, BLACK);
    y += 40;

    DrawText("Stations:", 20, y, 18, DARKBLUE);
    y += 24;

    for (const auto& s : snap.stations) {
        DrawText(TextFormat("Station %u type=%d waiting=%zu", s.id, (int) s.type, s.waiting), 40, y,
                 16, BLACK);
        y += 20;
    }

    y += 20;
    DrawText("Trains:", 20, y, 18, DARKGREEN);
    y += 24;

    for (const auto& t : snap.trains) {
        DrawText(TextFormat("Train %u line=%u station=%u onboard=%zu/%zu State=%u", t.id, t.lineId,
                            t.stationId, t.onboard, t.capacity, (int) t.state),
                 40, y, 16, BLACK);
        y += 20;
    }

    y += 20;
    DrawText("Passengers:", 20, y, 18, MAROON);
    y += 24;

    for (const auto& p : snap.passengers) {
        DrawText(TextFormat("P%u %d->%d state=%d age=%zu", p.id, (int) p.origin,
                            (int) p.destination, (int) p.state, p.age),
                 40, y, 16, BLACK);
        y += 18;
    }
}
