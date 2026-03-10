class Entity {
public:
	virtual ~Entity() = default;

    std::string name;
    Stats stats;
    int hp;

    virtual void take_dmg(val) {
        hp -= val;
    }
    
    virtual void heal(val) {
        if ((hp + val) > stats.max_hp) {
            hp = max_hp;
        } else if (hp >= 0) {
            return;
        } else {
            hp += val;
        }
    }

    virtual bool is_alive() {
        if (hp <= 0) {
            return false;
        } else {
            return true;
        }
    }
};
