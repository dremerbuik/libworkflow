#include <core/action.h>
#include <core/context.h>
#include <tools/property_set.h>
#include <core/request.h>
#include <core/session.h>
#include <core/put_definition.h>
#include <core/state_machine.h>


Action::Action(const std::string & name): action_id(0), name(name),
    properties(new PropertySet()) {
    propertyset->setGuarded(true);
    
}

Action::~Action() {
}

bool Action::checkInputs(SessionPtr session, ErrorReport & er) const {
    for(const auto & id: getInputs()) {
        auto ctx = session->getInput(getActionId(), id.put_name);
        
        if(not ctx) {
            // No context was provided, is it allowed ?
            if(id.mandatory) {
                std::stringstream str;
                str << fingerprint(session) << " No context provided for input " << id.put_name;
                er.setError("action.input.nil", str.str());
                return false;
            }
            return true;
        } else {
            // A context was provided. Does it match requirements.
            if(id.checker->check(ctx)) {
                // it matches requirement.
                if(id.ignoreEmpty) {
                    auto lctx = boost::dynamic_pointer_cast<ListCtx>(ctx);
                    if(lctx and lctx->isEmpty()) {
                        // this will refuse execution
                        // as the provided list is empty.
                        return false;
                    }
                }
                
                
            } else {
                
                std::stringstream str;
                str << fingerprint(session) << " Invalid context provided for input " << id.put_name << " (provided: " << ctx->getType() << ", expected: " << id.checker->generate()->getType() <<")";
                er.setError("action.input.invalid", str.str());
                return false;
            }
            
        }
    }
    
}

bool Action::checkOutputs(SessionPtr, ErrorReport &) const {
    for(const auto & id: getOutputs()) {
        auto ctx = session->getOutput(getActionId(), id.put_name);
        
        if(not ctx) {
            // No context was provided, is it allowed ?
            if(id.mandatory) {
                std::stringstream str;
                str << fingerprint(session) << " No context provided for output " << id.put_name;
                er.setError("action.output.nil", str.str());
                return false;
            }
            return true;
        } else {
            // A context was provided. Does it match requirements.
            if(id.checker->check(ctx)) {
                // it matches requirement.
                return true;
            } else {
                // test for skip !
                if(boost::dynamic_pointer_cast<SkipCtx>(ctx) and id.allowSkip) {
                    // ahah ! that's a skip
                    return true;
                }
                
                std::stringstream str;
                str << fingerprint(session) << " Invalid context provided for output " << id.put_name << " (provided: " << ctx->getType() << ", expected: " << id.checker->generate()->getType() <<")";
                er.setError("action.output.invalid", str.str());
                return false;
            }
            
        }
    }
}

Result Action::perform(SessionPtr) const {
    return error("action.perform.unimplemented","Can't execute this action as Perform function wasn't implemented.");
}

Result Action::replyReceived(SessionPtr, RequestPtr) const {
    return error("action.reply.unimplemented","Didn't expect a reply to this action.");
}

bool Action::canPerform(SessionPtr, ErrorReport &) const {
    return true;
}

const std::string & Action::getName() const{
    return name;
}

void Action::setName(const std::string & n) {
    name = n;
}

int32_t Action::getActionId() const {
    return action_id;
}

void Action::setActionId(int32_t aid) {
    action_id = aid;
}

PropertySetPtr Action::properties() {
    return propertyset;
}

StateMachinePtr Action::getStateMachine() const {
    return state_machine;
}

Result Action::done() const {
    
}

Result Action::wait() const {
    
}

Result Action::async() const {
    
}

Result Action::error(const ErrorReport &) const {
    
}

void Action::asyncDone() const {
    
}

void Action::asyncWait() const {
    
}

void Action::asyncError(const ErrorReport &) const {
    
}


std::string Action::fingerprint(SessionPtr) const {
    
}

double Action::doubleValue(SessionPtr session, const std::string & key, double def) const {
    // well def config is Session(byAction) > Session > Action
    
    if(session->getBypass(getActionId())->hasProperty(key))
        return session->getBypass(getActionId())->getDoubleValue(key, def);
    if(session->getBypass()->hasProperty(key)
       return session->getBypass()->getDoubleValue(key,def);
    if(propertyset->hasProperty(key))
       return propertyset->getDoubleValue(key, def);
       
    return def;
}

bool Action::boolValue(SessionPtr, const std::string & key, bool def) const {
    
    if(session->getBypass(getActionId())->hasProperty(key))
        return session->getBypass(getActionId())->getBoolValue(key, def);
    if(session->getBypass()->hasProperty(key)
       return session->getBypass()->getBoolValue(key,def);
       if(propertyset->hasProperty(key))
       return propertyset->getBoolValue(key, def);
       
       return def;
}

std::string Action::stringValue(SessionPtr, const std::string & key, const std::string & def ) const {
    
    if(session->getBypass(getActionId())->hasProperty(key))
        return session->getBypass(getActionId())->getStringValue(key, def);
    if(session->getBypass()->hasProperty(key)
       return session->getBypass()->getStringValue(key,def);
       if(propertyset->hasProperty(key))
       return propertyset->getStringValue(key, def);
       
       return def;
}

uint32_t Action::uintValue(SessionPtr, const std::string & key, uint32_t def) const {
    
    if(session->getBypass(getActionId())->hasProperty(key))
        return session->getBypass(getActionId())->getUintValue(key, def);
    if(session->getBypass()->hasProperty(key)
       return session->getBypass()->getUintValue(key,def);
       if(propertyset->hasProperty(key))
       return propertyset->getUintValue(key, def);
       
       return def;
}

ContextPtr Action::customValue(SessionPtr, const std::string & key, ContextPtr def) const {
    
    if(session->getBypass(getActionId())->hasProperty(key))
        return session->getBypass(getActionId())->getCustomValue(key, def);
    if(session->getBypass()->hasProperty(key)
       return session->getBypass()->getCustomValue(key,def);
       if(propertyset->hasProperty(key))
       return propertyset->getCustomValue(key, def);
       
       return def;
}

void Action::defineInput(const std::string & name, TypeCheckerPtr checker , bool mandatory) {
    PutDefinition d;
    d.put_name = name;
    d.checker = checker;
    d.mandatory = mandatory;
    
    defineInput(d);
}

void Action::defineInput(const PutDefinition & d) {
    //! @todo check that there aren't twice the same put name defined.
    
    inputs.insert(d);
}

void Action::defineOutput(const std::string & name, TypeCheckerPtr, bool mandatory) {
    
    PutDefinition d;
    d.put_name = name;
    d.checker = checker;
    d.mandatory = mandatory;
    
    defineOutput(d);
    
}

void Action::defineOutput(const PutDefinition &) {
    //! @todo check that there aren't twice the same put name defined.
    outputs.insert(d);
}

ContextPtr Action::getInput(SessionPtr, const std::string & name) {
    return session->getInput(getActionId(), name);
}

void Action::setOutput(SessionPtr, const std::string & name, ContextPtr ctx) {
    session->setOutput(getActionId(), name, ctx)
}

