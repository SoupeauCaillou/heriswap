package net.damsy.soupeaucaillou.heriswap.api;

import java.util.Map;

import net.damsy.soupeaucaillou.SacJNILib;

import com.swarmconnect.Swarm;
import com.swarmconnect.SwarmAchievement;
import com.swarmconnect.SwarmLeaderboard;

public class SuccessAPI {
	// -------------------------------------------------------------------------
		// SuccessAPI
		// -------------------------------------------------------------------------
		static public void unlockAchievement(int idS) {
			if (!Swarm.isInitialized() || !Swarm.isEnabled())
				return;
			final int id = idS;

			SwarmAchievement.GotAchievementsMapCB callback = new SwarmAchievement.GotAchievementsMapCB() {
				@Override
				public void gotMap(Map<Integer, SwarmAchievement> achievements) {
					if (achievements == null)
						return;
			        SwarmAchievement achievement = achievements.get(id);
			        // No need to unlock more than once...
			        if (achievement != null && achievement.unlocked == false) {
			            achievement.unlock();
			        }
				}
			};
			SwarmAchievement.getAchievementsMap(callback);
		}
 

		static public void openLeaderboard(int mode, int difficulty) {
			if (!Swarm.isInitialized()) {
				SacJNILib.activity.runOnUiThread(new Runnable() {
					@Override
					public void run() {
						Swarm.init(SacJNILib.activity, SacJNILib.activity.getSwarmGameID(), SacJNILib.activity.getSwarmGameKey());				
					}
				});
			} else {
				if (mode >= 0 && mode <= 2 && difficulty >= 0 && difficulty <= 2) {
					SwarmLeaderboard.GotLeaderboardCB callback = new SwarmLeaderboard.GotLeaderboardCB() {
					    public void gotLeaderboard(SwarmLeaderboard leaderboard) { 
					    	if (leaderboard != null) {
					    		leaderboard.showLeaderboard();
					        }
					    }
					};
	
					SwarmLeaderboard.getLeaderboardById(SacJNILib.activity.getSwarmBoards()[3 * mode + difficulty], callback);
				}
			}
		}

		static public void openDashboard() {
			if (!Swarm.isInitialized()) {
				SacJNILib.activity.runOnUiThread(new Runnable() {
					@Override
					public void run() {
						Swarm.init(SacJNILib.activity, SacJNILib.activity.getSwarmGameID(), SacJNILib.activity.getSwarmGameKey());				
					}
				});
			} else {
				Swarm.showDashboard();
			}
		}
}