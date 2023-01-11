#include "undo-redo/ur-commands/entity/drag-and-drop-entity-command.hpp"

namespace TWE {
    DragAndDropEntityCommand::DragAndDropEntityCommand(const Entity& entity, const Entity& to)
    : _entity(entity), _to(to) {
        _preParent = { _entity.getComponent<ParentChildsComponent>().parent, _entity.getScene() };
    }

    void DragAndDropEntityCommand::execute() {
        if(_preParent.getSource() != entt::null) {
            auto& childs = _preParent.getComponent<ParentChildsComponent>().childs;
            auto& itChild = std::find_if(childs.begin(), childs.end(), [&](entt::entity child) {
                return child == _entity.getSource();
            });
            if(itChild != childs.end())
                childs.erase(itChild);
            if(_to.getSource() != entt::null)
                _to.getComponent<ParentChildsComponent>().childs.push_back(_entity.getSource());
            else
                _entity.getComponent<ParentChildsComponent>().parent = entt::null;
        } else if(_to.getSource() != entt::null) {
            _entity.getComponent<ParentChildsComponent>().parent = _to.getSource();
            _to.getComponent<ParentChildsComponent>().childs.push_back(_entity.getSource());
        } else 
            _entity.getComponent<ParentChildsComponent>().parent = entt::null;
    }

    void DragAndDropEntityCommand::unExecute() {
        if(_to.getSource() == entt::null) {
            _entity.getComponent<ParentChildsComponent>().parent = _preParent.getSource();
            if(_preParent.getSource() != entt::null)
                _preParent.getComponent<ParentChildsComponent>().childs.push_back(_entity.getSource());
            return;
        }
        auto& childs = _to.getComponent<ParentChildsComponent>().childs;
        auto& itChild = std::find_if(childs.begin(), childs.end(), [&](entt::entity child) {
            return child == _entity.getSource();
        });
        if(itChild != childs.end())
            childs.erase(itChild);
        _entity.getComponent<ParentChildsComponent>().parent = _preParent.getSource();
        if(_preParent.getSource() != entt::null)
            _preParent.getComponent<ParentChildsComponent>().childs.push_back(_entity.getSource());
    }
}