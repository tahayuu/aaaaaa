# include <Siv3D.hpp> // Siv3D v0.6.14

// プレイヤーと敵の位置を表す構造体
struct Position {
	int32 x;
	int32 y;
};

// プレイヤーの弾を表す構造体
struct PlayerBullet {
	int32 x;
	int32 y;
	double angle; // 弾の発射角度（ラジアン）
};

// 敵を表す構造体（色の情報を追加）
struct Enemy {
	int32 x;
	int32 y;
	int32 speed;
	Color color;  // 敵の色
	int32 hitPoints; //耐久力（ヒットポイント）
	bool isStatic; //動かない敵かどうかのフラグ
};

// 敵の弾を表す構造体
struct EnemyBullet {
	int32 x;
	int32 y;
	double speed;
	double angle; // 弾の進行方向の角度（ラジアン）
};

// アイテムを表す構造体
struct Item {
	int32 x;
	int32 y;
	int32 type;
	int32 speed;
};

// プレイヤーの操作
void playerControl(Position& pos, int32 speed) {
	if (KeyW.pressed()) pos.y -= speed;
	if (KeyS.pressed()) pos.y += speed;
	if (KeyA.pressed()) pos.x -= speed;
	if (KeyD.pressed()) pos.x += speed;
}

// Shiftキーの押下状態によってスピードを変える
int32 getSpeed() {
	if (KeyShift.pressed()) return 10;
	else return 3;
}

// プレイヤーの移動範囲制限
void playerMovementRestriction(Position& pos) {
	if (pos.x < 0) pos.x = 0;
	if (pos.x > 800) pos.x = 800;
	if (pos.y < 0) pos.y = 0;
	if (pos.y > 600) pos.y = 600;
}

// 通常の敵を生成する関数
void spawnEnemy(Array<Enemy>& enemies) {
	int32 enemyX = Random(0, 800);
	enemies << Enemy{ enemyX, 0, 3, Palette::Red,1,false };  // 赤い敵を生成
}

// 青色の敵を生成する関数
void spawnEnemy2(Array<Enemy>& enemies) {
	int32 enemyX = 0;
	enemies << Enemy{ enemyX, 0, 3, Palette::Blue,1,false };  // 青い敵を生成
}

// 緑色の敵を生成する関数
void spawnEnemy3(Array<Enemy>& enemies) {
	int32 enemyX = Random(0,800);
	enemies << Enemy{ enemyX, 0, 2, Palette::Green, 3,false };  // 緑の敵を生成
}

// ボスを出現させる関数
void spawnBoss(Array<Enemy>& enemies,Vec2 bossPos) {
	enemies.clear(); // ほかの敵をすべて削除
	enemies << Enemy{ static_cast<int32>(bossPos.x), static_cast<int32>(bossPos.y), 0, Palette::Yellow, 100 };
}

//動かない敵を生成する関数
void spawnStaticEnemy(Array<Enemy>& enemies, Array<EnemyBullet>& enemyBullets) {
	enemies << Enemy{ 200, 100, 0, Palette::Purple, 2,true }; //動かない紫色の敵を生成
	enemies << Enemy{ 600,100,0,Palette::Purple,2,true }; 

		//生成した敵の位置を使って弾を発射する
		for (const auto& enemy : enemies) {
			if (enemy.isStatic) {

				enemyBullets << EnemyBullet{ enemy.x, enemy.y, 2.5, Math::ToRadians(120.0) };
				enemyBullets << EnemyBullet{ enemy.x, enemy.y, 2.5, Math::ToRadians(60.0) };
				enemyBullets << EnemyBullet{ enemy.x, enemy.y, 2.5 , Math::ToRadians(90.0) };
			}
		}
}

void spawnStaticEnemy2(Array<Enemy>& enemies, Array<EnemyBullet>& enemyBullets) {
	enemies << Enemy{ 200,100,0,Palette::Orange,3,true };//動かないオレンジの敵の生成
	enemies << Enemy{ 600,100,0,Palette::Orange,3,true };

	for (const auto& enemy : enemies)
	{
		if (enemy.isStatic) {
			if (RandomBool(0.30)) {
				// 扇状に弾を発射
				for (int i = 0; i < 5; ++i) {
					double angle = Math::ToRadians(90.0) + Math::ToRadians(i * 15.0 - 30.0); // 5発の弾が扇状に発射
					enemyBullets << EnemyBullet{ enemy.x, enemy.y, 5, angle };
				}
			}

		}
	}
}


			


//ボスのヒットポイントによる攻撃パターン変更
void updateBossAttack(Enemy& boss, Array<EnemyBullet>& enemyBullets,double& bossAttackTimer,double attackInterval,Array<Enemy>& enemies) {
	double currentTime = Scene::Time();

	//HPに応じて発射間隔を変更
	if (boss.hitPoints > 70) {
		attackInterval = 1.3;
	}
	else if (boss.hitPoints > 30) {
		attackInterval = 1.5;
	}
	else {
		attackInterval = 1.5;
	}
	//前回の発射から一定間隔が経過したら弾を発射する
	if (currentTime - bossAttackTimer > attackInterval) {

		//ボスのHPに基づいてフェーズを変更
		if (boss.hitPoints > 70) {
			//フェーズ１：扇状に弾を発射
			for (int i = 0; i < 5; ++i) {
				double angle = Math::ToRadians(90.0) + Math::ToRadians(i * 15.0 - 30.0); // 5発の弾が扇状に発射
				enemyBullets << EnemyBullet{ boss.x, boss.y, 5, angle };
			}


			if (RandomBool(0.3)) {
				spawnStaticEnemy(enemies, enemyBullets);
			}
		}
		else if (boss.hitPoints > 30) {
			//画面全体に弾を発
			int numBullets = 12;  // 発射する弾の数
			double baseAngle = Math::ToRadians(360.0 / numBullets); // 各弾の角度の間隔

			for (int i = 0; i < numBullets; ++i) {
				double angle = baseAngle * i;
				enemyBullets << EnemyBullet{ boss.x, boss.y, 4, angle };
			}

			if (RandomBool(0.3)) {
				spawnStaticEnemy2(enemies, enemyBullets);
			}

		}

		else {
			//フェーズ３：３６０度に広がる弾幕
			for (int i = 0; i < 360; i += 8) {  // 360度に弾を発射
				double angle = Math::ToRadians(i);
				enemyBullets << EnemyBullet{ boss.x, boss.y, 4, angle };
			}
			if (RandomBool(0.15)) {
				spawnStaticEnemy2(enemies, enemyBullets);
			}
		}
		//発射タイマーをリセット（現在の時間に更新）
		bossAttackTimer = currentTime;
	}
}

	// 敵を移動して描画する関数
	void updateEnemies(Array<Enemy>&enemies, Array<EnemyBullet>&enemyBullets,double& bossAttackTimer,double attackInterval, const Texture& enemyTexture) {
		for (Enemy& enemy : enemies) {
			// ボス（耐久力50以上）

			if (enemy.hitPoints > 50) {
				enemy.x += enemy.speed;

					updateBossAttack(enemy, enemyBullets, bossAttackTimer, attackInterval, enemies);
			}
			//ボスの第2形態の実装  
			else if (enemy.hitPoints <= 30 && enemy.color == Palette::Yellow) {
				if (RandomBool(0.03)) {
					for (int i = 0; i < 360; i += 5) { // 360度に弾を発射
						double angle = Math::ToRadians(i);
						enemyBullets << EnemyBullet{ enemy.x, enemy.y, 4, angle };
					}
				}
			}

			// 通常の敵の移動と描画
			if (enemy.color == Palette::Blue) {
				enemy.x += enemy.speed;

			}
		

			else {
				enemy.y += enemy.speed;
				
			}

			// 通常の敵の弾発射
			if (RandomBool(0.02)) {
				if (enemy.color == Palette::Blue) {
					enemyBullets << EnemyBullet{ enemy.x, enemy.y, 5, Math::ToRadians(120.0) };
					enemyBullets << EnemyBullet{ enemy.x, enemy.y, 5, Math::ToRadians(60.0) };
					if (RandomBool(0.05)) {
						// 扇状に弾を発射
						for (int i = 0; i < 5; ++i) {
							double angle = Math::ToRadians(90.0) + Math::ToRadians(i * 15.0 - 30.0); // 5発の弾が扇状に発射
							enemyBullets << EnemyBullet{ enemy.x, enemy.y, 5, angle };
						}
					}
				}
				else if (enemy.color == Palette::Green) {
					enemyBullets << EnemyBullet{ enemy.x, enemy.y, 5, Math::ToRadians(120.0) };
					enemyBullets << EnemyBullet{ enemy.x, enemy.y, 5, Math::ToRadians(60.0) };
					enemyBullets << EnemyBullet{ enemy.x, enemy.y, 5, Math::ToRadians(90.0) };
				}
				else if (enemy.color == Palette::Red) {
					enemyBullets << EnemyBullet{ enemy.x, enemy.y, 5, Math::ToRadians(90.0) };
				}
			}
		}
	}



//ボスの体力を描画する関数
void drawBossHealthBar(const Enemy& boss, const Texture& bossLifeTexture) {
	const int32 maxHealth = 100;
	const int32 barWidth = 400;
	const int32 barHeight = 20;
	double healthRatio = static_cast<double>(boss.hitPoints) / maxHealth;
	int32 currentBarWidth = static_cast<int32>(barWidth * healthRatio);
	//体力バーの背景を描画（灰色の枠）
	Rect{ 200,10,barWidth,barHeight }.draw(ColorF{ 0.25 });
	//現在の体力に応じたバーを描画（赤色）
	Rect{ 200,10,currentBarWidth,barHeight }.draw(Palette::Red);
	//体力バーの枠線を描画
	Rect{ 200,10,barWidth,barHeight }.drawFrame(2, 0, Palette::Black);
	// ボスの画像を体力バーの右側に表示
	bossLifeTexture.resized(60, 60).draw(Vec2(590, 0));  // 体力バーの横に画像を表示
}

void spawnMiddleEnemy(Array<Enemy>& enemies) {
	int32 enemyX = 400;  // 真ん中のX座標（中央に移動）
	int32 enemyY = 400;  // 真ん中のY座標（中央の位置）
	enemies << Enemy{ enemyX, enemyY, Random(1, 3), Palette::Red };  // 赤い敵を真ん中に配置
}

// 弾と敵が当たったかをチェックして、当たったら消す関数
void checkBulletCollisions(Array<PlayerBullet>& bullets, Array<Enemy>& enemies, Array<Item>& items) {
		for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
		bool hit = false;
		for (auto enemyIt = enemies.begin(); enemyIt != enemies.end();) {
			if (enemyIt->hitPoints > 50) {
				// ボスの当たり判定を長方形で行う
				Rect bossRect(enemyIt->x - 50, enemyIt->y - 100, 100, 200);
				if (bossRect.intersects(Circle{ bulletIt->x, bulletIt->y, 10 })) {
					enemyIt->hitPoints--; // 敵の耐久力を減らす
					hit = true;
				}
			}
			if (enemyIt->hitPoints <= 50 && enemyIt->color == Palette::Yellow) {
				//Circle{ enemyIt->x,enemyIt->y,45 }.draw(enemyIt->color);
				if (Circle{ bulletIt->x, bulletIt->y, 10 }.intersects(Circle{ enemyIt->x, enemyIt->y,45 })) {
					enemyIt->hitPoints--;
					hit = true;
				}
			}
			// ボスが倒れた場合の処理
			if (enemyIt->hitPoints <= 0 && enemyIt->color == Palette::Yellow) {
				enemyIt = enemies.erase(enemyIt);  // ボスが倒れたら配列から削除
				continue;  // 次の敵へ
			}

			if (Circle{ bulletIt->x, bulletIt->y, 10 }.intersects(Circle{ enemyIt->x, enemyIt->y, 20 })) {
				enemyIt->hitPoints--; // 敵の耐久力を減らす  

				if (enemyIt->hitPoints <= 0) {
					double dropChance = Random(); // 0.0から1.0の間のランダムな値を生成
					if (dropChance < 0.15) {
						items << Item{ enemyIt->x, enemyIt->y, 1, 2 }; // タイプ1（残機を増やす）
					}
					else if (dropChance < 0.25) {
						items << Item{ enemyIt->x, enemyIt->y, 0, 2 }; // タイプ0（弾数の増加）
					}
					enemyIt = enemies.erase(enemyIt); // 敵を削除
				}
				else {
					++enemyIt;
				}
				hit = true;
				break;
			}
			else {
				++enemyIt;
			}
		}
		if (hit) {
			bulletIt = bullets.erase(bulletIt); // 弾を削除
		}
		else {
			++bulletIt;
		}
	}
}

// プレイヤーがアイテムを取得したかチェックする関数
void checkItemPickup(Position& playerPos, Array<Item>& items, int& bulletSpread, int& lives) {
	for (auto itemIt = items.begin(); itemIt != items.end();) {
		if (Circle{ itemIt->x, itemIt->y, 10 }.intersects(Circle{ playerPos.x, playerPos.y, 30 })) {
			if (itemIt->type == 1) {
				lives++;
			}
			else {
				bulletSpread++;
			}
			itemIt = items.erase(itemIt);
		}
		else {
			++itemIt;
		}
	}
}


// 敵の弾を移動して描画する関数（角度を考慮）
void updateEnemyBullets(Array<EnemyBullet>& enemyBullets,const Texture&enemyBulletTexture){
	for (EnemyBullet& bullet : enemyBullets) {
		bullet.x += static_cast<int32>(bullet.speed * std::cos(bullet.angle));
		bullet.y += static_cast<int32>(bullet.speed * std::sin(bullet.angle));
		enemyBulletTexture.resized(30, 30).drawAt(Vec2(bullet.x, bullet.y));


	}
}

// 敵の弾がプレイヤーに当たったかどうかを判定する関数
bool checkPlayerHit(Position& playerPos, Array<EnemyBullet>& enemyBullets) {
	for (auto bulletIt = enemyBullets.begin(); bulletIt != enemyBullets.end();) {
		if (Circle{ bulletIt->x, bulletIt->y, 10 }.intersects(Circle{ playerPos.x, playerPos.y, 30 })) {
			bulletIt = enemyBullets.erase(bulletIt);  // 弾を削除
			return true;  // プレイヤーが被弾したことを返す
		}
		else {
			++bulletIt;  // 次の弾に進む
		}
	}
	return false;  // どの弾も当たらなかった
}
// メイン関数
void Main() {
	Position playerPos{ 400, 500 };  // プレイヤーの初期位置
	Array<PlayerBullet> bullets;  // 弾の座標と角度を格納する配列
	Array<Enemy> enemies;  // 敵の配列
	Array<EnemyBullet> enemyBullets;  // 敵の弾の配列
	Array<Item> items;  // ドロップされたアイテムの配列

	const Texture itemTexture1(U"assets/images/item1.png");  // アイテム1のテクスチャ
	const Texture itemTexture2(U"assets/images/item2.png");  // アイテム2のテクスチャ
	const Texture bossTexture(U"assets/images/boss.png");
	const Texture playerTexture(U"assets/images/player.png");
	const Texture enemyTexture(U"assets/images/enemy.png");
	const Texture playerBulletTexture(U"assets/images/Bullet1.png");
	const Texture enemyBulletTexture(U"assets/images/Bullet2.png");
	const Texture enemy2Texture(U"assets/images/enemy2.png");
	const Texture enemy3Texture(U"assets/images/enemy3.png");
	const Texture lifeTexture(U"assets/images/life.png");
	const Texture bossLifeTexture(U"assets/images/boss.life.png");
	bool isStartScreen = true;//ゲームがスタートしているかどうか管理するフラグ

	Vec2 bossPos = Vec2(400, 130);//ボスの位置
	int lives = 99;  // プレイヤーの残機を追加
	bool isInvincible = false;  // 無敵状態のフラグ
	double invincibleTimer = 0.0;  // 無敵状態のタイマー
	const double invincibleDuration = 2.0;  // 無敵状態の継続時間（秒）
	int bulletSpread = 1;
	double bossAttackTimer = 0.0;//ボスの弾発射タイマー
	const double bossAttackInterval2 = 1.0;
	bool bossSpawned = false;  // ボスが出現したかどうかを管理するフラグ
	double attackInterval = 2.0;  // ボスの攻撃間隔（初期値

	double enemySpawnTimer = 0.0;  // 敵出現のタイマー
	double enemySpawnTimer2 = 0.0;  // 青色の敵出現のタイマー
	double enemySpawnTimer3 = 0.0;  // 緑色の敵出現タイマー
	double bossSpawnTime = 60.0;  // ボス出現までの時間（秒）
	const Font font(30);
	const Font smallFont(20);  // フォントサイズを 15 に設定
	const Font titleFont(60); //タイトル用の大きなフォント
	const Font infoFont(30);  //説明用のフォント

	if (!bossTexture) {
		Print << U"Failed to load boss.png";
	}
	if (!playerTexture) {
		Print << U"Failed to load player.png";
	}
	if (!enemyTexture) {
		Print << U"Failed to load enemy.png";
	}

	for (const auto& enemy : enemies) {
		Print << U"Enemy Position: " << enemy.x << U"," << enemy.y;
	}

	// 敵の出現を管理するタイマー
	double enemySpawnInterval = 1.0;  // 敵が出現する間隔

	while (System::Update()) {
		if (isStartScreen) {
			//スタート画面の描画
			titleFont(U"スペーストリガー").drawAt(Scene::Center().movedBy(0, -100), Palette::White);//タイトル表示
			infoFont(U"wasdで移動").draw(Vec2(300, 300), Palette::White);
			smallFont(U"shiftキーで移動速度up").draw(Vec2(300, 320), Palette::White);
			smallFont(U"spaceキーで発射").draw(Vec2(300, 340), Palette::White);
			infoFont(U"Enterキーで開始").draw(Vec2(300, 520), Palette::Red);
			lifeTexture.resized(100, 100).draw(Vec2(120, 300));
			bossLifeTexture.resized(100, 100).draw(Vec2(680, 300));

			if (KeyEnter.down()) {
				isStartScreen = false;
				playerPos = { 400, 500 };  // プレイヤーの位置を再設定
				lives = 99;  // 残機をリセット
			}
		}
		else {
			playerControl(playerPos, getSpeed());  // プレイヤーの操作
			playerMovementRestriction(playerPos);  // プレイヤーの移動範囲制限

			// プレイヤーの描画（無敵状態なら点滅させる）
			if (!isInvincible || static_cast<int>(Scene::Time() * 10) % 2 == 0) {
				playerTexture.resized(80, 80).drawAt(Vec2(playerPos.x, playerPos.y));
			}

			// スペースキーで弾を発射
			if (KeySpace.down()) {
				for (int i = 0; i < bulletSpread; ++i) {
					double angle = Math::ToRadians(-90.0) + Math::ToRadians(30.0 * i - 15.0 * (bulletSpread - 1));
					bullets << PlayerBullet{ playerPos.x, playerPos.y, angle };
				}
			}

			// アイテムの移動と描画（追加部分）
			for (Item& item : items) {
				item.y += item.speed;  // アイテムを下に移動

				// アイテムの種類に応じたテクスチャを描画
				if (item.type == 1) {
					itemTexture2.drawAt(Vec2(item.x, item.y));  // 1UPアイテム
				}
				else if (item.type == 0) {
					itemTexture1.drawAt(Vec2(item.x, item.y));  // 弾数増加アイテム
				}
			}
			// 弾の移動と描画
			for (PlayerBullet& bullet : bullets) {
				bullet.x += static_cast<int32>(10 * std::cos(bullet.angle));
				bullet.y += static_cast<int32>(10 * std::sin(bullet.angle));
				playerBulletTexture.resized(30, 30).drawAt(Vec2(bullet.x, bullet.y));
			}


			// 敵の生成
			if (!bossSpawned) {
				enemySpawnTimer += Scene::DeltaTime();
				if (enemySpawnTimer > 1.0) {
					spawnEnemy(enemies);  // 赤い敵を生成
					enemySpawnTimer = 0.0;
				}

				enemySpawnTimer2 += Scene::DeltaTime();
				if (enemySpawnTimer2 > 4.0) {
					spawnEnemy2(enemies);  // 青い敵を生成
					enemySpawnTimer2 = 0.0;
				}

				enemySpawnTimer3 += Scene::DeltaTime();
				if (enemySpawnTimer3 > 8.0) {
					spawnEnemy3(enemies);  // 緑色の敵を生成      
					enemySpawnTimer3 = 0.0;
				}
			}

			if (bossSpawned && !enemies.isEmpty() && enemies[0].color == Palette::Yellow) {
				drawBossHealthBar(enemies[0], bossLifeTexture);//enemies配列の最初の敵がボスであることを仮定
			}
			// 無敵状態のタイマーを更新 
			if (isInvincible) {
				invincibleTimer -= Scene::DeltaTime();
				if (invincibleTimer <= 0.0) {
					isInvincible = false;
				}
			}

			// ボスの出現処理
			if (!bossSpawned && Scene::Time() >= bossSpawnTime) {
				spawnBoss(enemies, bossPos);
				spawnMiddleEnemy(enemies);   // 敵を真ん中に配置
				bossSpawned = true;
			}

			if (bossSpawned && !enemies.isEmpty() && enemies[0].color == Palette::Yellow) {
				// ボスの攻撃を更新（ボスの弾発射を管理）
				updateBossAttack(enemies[0], enemyBullets, bossAttackTimer, attackInterval, enemies);

				// ボスの体力バーを描画
				drawBossHealthBar(enemies[0], bossLifeTexture);
			}

			// 敵のテクスチャを描画（最初の敵のみ例示）
			for (const auto& enemy : enemies) {
				if (enemy.color == Palette::Blue || enemy.color == Palette::Orange) {
					// 青い敵のテクスチャで描画
					enemy2Texture.resized(90, 90).drawAt(Vec2(enemy.x, enemy.y));
				}
				else if (enemy.color == Palette::Green || enemy.color == Palette::Purple) {
					// 緑の敵のテクスチャで描画
					enemy3Texture.resized(90, 90).drawAt(Vec2(enemy.x, enemy.y));
				}
				else {
					enemyTexture.resized(90, 90).drawAt(Vec2(enemy.x, enemy.y));
				}
			}


			// ボスのテクスチャを描画
			if (bossSpawned) {
				bossTexture.drawAt(bossPos);


			}



			// 敵の移動と描画
			updateEnemies(enemies, enemyBullets, bossAttackTimer, attackInterval, enemyTexture);



			// 弾と敵の当たり判定
			checkBulletCollisions(bullets, enemies, items);

			// 敵の弾とプレイヤーの当たり判定
			if (!isInvincible && checkPlayerHit(playerPos, enemyBullets)) {
				lives--;  // プレイヤーの残機を減らす
				isInvincible = true;  // 無敵状態を開始
				invincibleTimer = invincibleDuration;  // 無敵状態の時間をセット
				if (lives <= 0 || (bossSpawned && !enemies.isEmpty() && enemies[0].hitPoints == 0)) {
					System::Exit();  // 残機が0になったらゲーム終了
				}
			}

			// 敵の弾を移動して描画する関数（角度を考慮）
			updateEnemyBullets(enemyBullets, enemyBulletTexture);


			// プレイヤーとアイテムの当たり判定
			checkItemPickup(playerPos, items, bulletSpread, lives);

			// 残機を画面に表示
			font(U"残機:{}"_fmt(lives)).draw(Vec2(10, 10), Palette::White);
			lifeTexture.resized(40, 40).draw(Vec2(120, 10));  // 画像の大きさや位置は調整してください
			//説明表示
			smallFont(U"wasdキーで移動").draw(Vec2(600, 10), Palette::Red);
			smallFont(U"spaceキーで弾を発射").draw(Vec2(600, 30), Palette::Red);
			smallFont(U"shiftキーで速度up").draw(Vec2(600, 50), Palette::Red);
		}
	}
}
