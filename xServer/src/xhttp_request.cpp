//
// Created by lyn on 2023/2/23.
//
#include "xhttp_request.h"
bool xyq::xhttp_request::get_enable() const {
    return this->enable;
}

void xyq::xhttp_request::set_header(std::string key, std::string value) {
    if (key.size() == 0 and value.size() == 0)
        return;
    this->req_header[key] = value;
}

void xyq::xhttp_request::set_header(const std::pair<std::string, std::string> &kv) {
    this->set_header(kv.first, kv.second);
}

std::string xyq::xhttp_request::get_header(std::string header_key) {
    if (this->req_header.find(header_key) == this->req_header.end()) {
        return "";
    } else {
        return this->req_header[header_key];
    }
}



void xyq::xhttp_request::ana_url() {
    char rest[HTTP_URL_SIZE];
    char path[HTTP_URL_SIZE];
    char key[HTTP_URL_SIZE];
    char value[HTTP_URL_SIZE];
    if (xyq::divide_str_by_separator(this->url.data(), path, rest, "?", sizeof(path), sizeof(rest)) == 0) {
        this->path = path;
        while (xyq::divide_str_by_separator(rest, value, rest, "&", sizeof(value), sizeof(rest)) == 0) {
            if (xyq::divide_str_by_separator(value, key, value, "=", sizeof(key), sizeof(value)) == 0) {
                this->params[key] = value;
            }
        }
        if (xyq::divide_str_by_separator(value, key, value, "=", sizeof(key), sizeof(value)) == 0) {
            this->params[key] = value;
        }
    } else {
        this->path = this->url;
    }
}

std::string xyq::xhttp_request::get_method() const {
    return this->method;
}

std::string xyq::xhttp_request::get_ip() const {
    return this->ip;
}

std::string xyq::xhttp_request::get_version() const {
    return this->http_version;
}

std::string xyq::xhttp_request::get_path() {
    return this->path;
}

std::string xyq::xhttp_request::text() {
    return this->req_content;
}

void xyq::xhttp_request::show_info() const {
    cout<<this->method<<" "<<this->path<<" "<<this->http_version<<endl;
}